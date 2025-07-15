#include <kuel/matcher.hpp>
#include <iostream>
#include <string>

namespace kuel{

namespace matcher{

std::ostream& operator<<(std::ostream& os, const state& s) {
    if (s.type >= T_ELEMENT)
        return os << "STATE(" << s.type << ',' << (char)s.chr << ',' << s.quantifier << ")";
    os << "STATE(" << s.type << ',' << s.quantifier << (s.token.empty() ? "" : ","+s.token) << "){ ";
    for (const auto& s : *s.states)
        os << s << ", ";
    return os << " }";
}

state& state::operator+=(
    const state& state_ptr
) {
    states->push_back(state_ptr);
    return *this;
}

matcher::matcher(std::string src) {
    _root = new state(T_GROUP, Q_ONE);
    _parse(src, *_root);
    std::cout << *_root << '\n';
}

matcher::~matcher() {
    _destroy();
}

bool matcher::operator()(std::string pred) {
    _curr_tr = test_result(pred);
    _test(*_root);
    std::cout << "\n";
    std::cout << "Terminated at " << _curr_tr.i() << '\n';
    return _curr_tr.get();
}

int matcher::_parse(std::string& src, state& parent_state, size_t i) {
    if (i >= src.size()) return --i;
    const char c = src[i++];
    switch (c) {
        case '(': {
            state* curr = new state(T_GROUP, Q_ONE);
            i = _parse(src, *curr, i);
            parent_state += (*curr);
            delete curr;
            break;
        }
        case ')': return i;
        case '[': {
            state* curr = new state(T_MAP, Q_ONE);
            i = _parse(src, *curr, i);
            parent_state += (*curr);
            delete curr;
            break;
        }
        case ']': return i;
        case '<': {
            std::string s = "";
            while(src[i]!='>') s+=src[i++];
            parent_state.token = s;
            ++i;
            break;
        }
        case '.': {
            state* curr = new state(T_WILDCARD, Q_ONE, c);
            parent_state += (*curr);
            delete curr;
            break;
        }
        case '\\': {
            state* curr = new state((state_type)src[i],Q_ONE,src[i]);
            ++i;
            parent_state += (*curr);
            delete curr;
            break;
        }
        case '?': case '*': case '+': {
            state& curr = parent_state.states->back();
            if (c != '+') {
                curr.quantifier = (state_quantifier)c;
                break;
            }
            curr.quantifier = Q_ONE;
            state* next = nullptr;
            if (curr.type < T_ELEMENT)
                next = new state(curr.type,Q_ZEROMANY,curr.states);
            else
                next = new state(curr.type,Q_ZEROMANY,curr.chr);
            parent_state += (*next);
            break;
        }
        default: {
            state* curr = new state(T_ELEMENT,Q_ONE,c);
            parent_state += (*curr);
            delete curr;
            break;
        }
    }
    return _parse(src, parent_state, i);
}

bool matcher::_test_type(const state& curr) {
    const char ch = _curr_tr.next();
    switch (curr.type) {
        case T_ELEMENT: return ch == curr.chr;
        case T_ALPHA: return isalpha(ch);
        case T_ALPHANUM: return isalnum(ch);
        case T_DIGIT: return isdigit(ch);
        case T_WHITESPACE: return isspace(ch);
        case T_WILDCARD: return true;
        case T_GROUP: {
            // test_result snap_tr = _curr_tr;
            for (auto& s : *curr.states) {
                std::cout << "GROUP_TEST[ " << _curr_tr.i() << " " << _curr_tr.get() << " " << s << " ]\n";
                _test(s);
                std::cout << "GROUP_TEST_END[ " << _curr_tr.i() << " " << _curr_tr.get() << " ]\n";
                if (_curr_tr.get()) continue;
                std::cout << "GROUP_BCKTRK[ " << _curr_tr.i() << " " << _curr_tr.get() << " ]\n";
                _backtrack(s);
                std::cout << "GROUP_BCKTRK_END[ " << _curr_tr.i() << " " << _curr_tr.get() << " ]\n";
                if (!_curr_tr.get()) {
                    // _restore(snap_tr);
                    return false;
                }
            }
            return true;
        }
        case T_MAP: {
            // test_result snap_tr = _curr_tr;
            for (auto& s : *curr.states) {
                _curr_tr.set();
                std::cout << "MAP_TEST[ " << _curr_tr.i() << " " << _curr_tr.get() << " " << s << " ]\n";
                _test(s);
                std::cout << "MAP_TEST_END[ " << _curr_tr.i() << " " << _curr_tr.get() << " ]\n";
                if (_curr_tr.get()) return true;
            }
            std::cout << "MAP_BCKTRK[ " << _curr_tr.i() << " " << _curr_tr.get() << " ]\n";
            auto prev_tr = _backtrack(curr);
            std::cout << "MAP_BCKTRK_END[ " << _curr_tr.i() << " " << _curr_tr.get() << " ]\n";
            return _curr_tr.get();
            // if (_curr_tr.get()) return true;
            // _restore(snap_tr);
            // return false;
        }
        default: break;
    }
    return false;
}

void matcher::_test(const state& curr_state) {
    switch (curr_state.quantifier) {
        case Q_ONE:
            if (!_curr_tr.next() || !_test_type(curr_state)) {
                _curr_tr.unset();
                return;
            }
            _curr_tr += curr_state;
            break;
        case Q_ZEROMANY:
            if (!_curr_tr.next() || !_test_type(curr_state)) return;
            _curr_tr += curr_state;
            _test(curr_state);
            break;
        case Q_ZEROONE:
            if (!_curr_tr.next()) return;
            if (_test_type(curr_state)) {
                _curr_tr += curr_state;
                if (_test_type(curr_state))
                    _curr_tr += curr_state;
            }
            break;
        default:
            break;
    }
}

test_result* matcher::_backtrack(
    const state& curr_state,
    test_result* tr
) {
    if (!_curr_tr.has_backtrackable()) { return tr; }
    if (tr == nullptr) tr = new test_result(_curr_tr);
    _curr_tr.backtrack();
    _test(curr_state);
    if (_curr_tr.get()) return nullptr;
    return _backtrack(curr_state, tr);
}

void matcher::_restore(
    const test_result& tr
) {
    _curr_tr.restore(tr);
}

void matcher::_destroy() {
    for (const auto& p : _state_vecs)
        delete p.first;
}

}
}
