#pragma once

#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>

namespace kuel {
namespace lexer {

enum struct StateType { ELEMT, WCARD, ALPHA, DIGIT };

enum struct StateQuantifier { EO, ZO, OM, ZM };

enum struct TokenType { KWD_EXIT, LTR_INT, SYB_BLKS, SYB_BLKE, EOL };

const char* st_to_str(StateType t);
const char* sq_to_str(StateQuantifier q);
const char* tt_to_str(TokenType t);

struct State {
  State() {}
  State(char ch, StateType type, StateQuantifier quant)
      : c(ch), t(type), q(quant) {}
  char c;
  std::list<std::shared_ptr<State>> next;
  StateType t;
  StateQuantifier q;
  std::optional<TokenType> acceptor;

  friend std::ostream& operator<<(std::ostream& os, const State& s);
};

struct Token {
  TokenType type;
  std::string value;
};

class Lexer;

class MatchTree {
 public:
  MatchTree();
  void append(const std::string& src, TokenType type);
  friend Lexer;

 private:
  std::shared_ptr<State> root_;

  void parse(const std::string& src, size_t idx, TokenType type,
             State& curr_state);
  friend std::ostream& operator<<(std::ostream& os, const MatchTree& m);
};

class Lexer {
 public:
  Lexer(std::unordered_map<std::string, TokenType>& patterns);
  bool match(char chr);
  std::list<Token> get();
  friend std::ostream& operator<<(std::ostream& os, const Lexer& matcher);

 private:
  MatchTree tree_;
  std::shared_ptr<State> curr_state_;
  std::string tkn_value_;
  bool is_blank_ = true;
  std::list<Token> tokens_;

  bool match_state(char chr, std::shared_ptr<State> st);
  bool match_state_type(char ch, const State& st);
  void reset();
};

}  // namespace lexer
}  // namespace kuel
