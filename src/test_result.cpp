#include <kuel/matcher.hpp>
#include <iostream>

namespace kuel{

namespace matcher{

test_result::test_result() : _i(0), _is_valid(true) {}

test_result::test_result(test_result& tr) : _i(tr._i), _is_valid(tr._is_valid), _curr_str(tr._curr_str), _history(tr._history) {}

test_result::test_result(std::string& pred)
    : _i(0), _is_valid(true), _curr_str(pred) {}

test_result& test_result::operator+=(
    const state& state_ptr
) {
    if (state_ptr.type > 1)
        ++_i;
    history_push(state_ptr);
    return *this;
}

// test_result& test_result::operator=(
//     test_result tr
// ) {
//     _i = tr._i;
//     _curr_str = tr._curr_str;
//     _is_valid = tr._is_valid;
//     _history = tr._history;
//     return *this;
// }

void test_result::set() { _is_valid = true; }

void test_result::unset() { _is_valid = false; }

bool test_result::get() { return _is_valid; }

size_t test_result::i() { return _i; }

void test_result::backtrack(
    // std::vector<consumed_state>* bt_disposed_cs
) {
    set();
    auto& cs = _history.back();
    _i = cs.count + cs.offset - 1;
    --cs.count;
    // if (
    //     !bt_disposed_cs->empty() &&
    //     bt_disposed_cs->back().offset == cs.offset
    // ) ++bt_disposed_cs->back().count;
    // else {
    //     consumed_state csn = {
    //         cs.ref,
    //         cs.is_backtrackable,
    //         cs.count, cs.offset,
    //         cs.last_bt_cs_i
    //     };
    //     bt_disposed_cs->push_back(csn);
    // }
    if (cs.count == 0)
        _history.pop_back();
}

void test_result::restore(
    const test_result& tr
) {
    _i = tr._i + 1;
    _curr_str = tr._curr_str;
    _is_valid = true;
    _history = tr._history;
}

void test_result::history_push(const state& state_ptr) {
    if (!_history.empty() && &state_ptr == _history.back().ref) {
        _history.back().count += 1;
        return;
    }
    int lbkcsi = state_ptr.quantifier ?
        _i - 1 :
        (!_history.empty() ? _history.back().last_bt_cs_i : -1);
    consumed_state cs = {
        &state_ptr,
        state_ptr.quantifier!=Q_ONE,
        1, _i - 1,
        lbkcsi
    };
    _history.push_back(cs);
}

bool test_result::has_backtrackable() {
    return !_history.empty() && _history.back().last_bt_cs_i != -1;
}

char test_result::next() {
    return _i < _curr_str.size() ? _curr_str[_i] : '\0';
}

}
}
