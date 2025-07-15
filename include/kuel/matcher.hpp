#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

namespace kuel {
namespace matcher {
enum state_type {
    T_TOKEN,
    T_GROUP,
    T_MAP,
    T_ELEMENT,
    T_WILDCARD,
    T_ALPHA = 'a',
    T_ALPHANUM = 'b',
    T_WHITESPACE = 'w',
    T_DIGIT = 'd'
};

enum state_quantifier {
    Q_ONE,
    Q_ZEROMANY = '*',
    Q_ONEMANY = '+',
    Q_ZEROONE = '?'
};

class state {
public:
    char chr;
    state_type type;
    std::vector<state>* states;
    state_quantifier quantifier;
    std::string token;
    state(state_type t, state_quantifier q) : type(t), quantifier(q), states(new std::vector<state>) {}
    state(state_type t, state_quantifier q, char c) : state(t, q) { chr = c; }
    state(state_type t, state_quantifier q, std::vector<state>* s) : state(t, q) { states = s; }
    state& operator+=(const state& state_ptr);
    friend std::ostream& operator<<(std::ostream& os, const state& s);
};

struct consumed_state {
    const state* ref;
    bool is_backtrackable;
    size_t count, offset;
    int last_bt_cs_i;
};

class test_result {
    size_t _i;
    bool _is_valid;
    std::string _curr_str;
    std::vector<consumed_state> _history;
public:
    test_result();
    test_result(std::string& pred);
    test_result(test_result& tr);
    test_result& operator+=(const state& state_ptr);
    void set();
    void unset();
    bool get();
    size_t i();
    void backtrack();
    void restore(const test_result& tr);
    void history_push(const state& state_ptr);
    bool has_backtrackable();
    char next();
};


class matcher {
private:
    state* _root;
    std::unordered_map<state*, bool> _state_vecs;
    int _parse(std::string& src, state& parent_state, size_t i = 0);
    test_result _curr_tr;

    bool _test_type(const state& curr);
    test_result* _backtrack(const state& curr_state, test_result* tr = nullptr);
    void _restore(const test_result& tr);
    void _test(const state& curr_state);
    void _destroy();
public:
    matcher(std::string src);
    bool operator()(std::string pred);
    ~matcher();
};

}
}
