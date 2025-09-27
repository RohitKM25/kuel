#include <kuel/lexer.hpp>

namespace kuel {

namespace lexer {

Lexer::Lexer(std::unordered_map<std::string, TokenType>& patterns) {
  for (const auto& pattern_p : patterns) {
    tree_.append(pattern_p.first, pattern_p.second);
  }
  curr_state_ = tree_.root_;
}

bool Lexer::match(char chr) {
  if (std::isblank(chr)) {
    if (is_blank_) return true;
    is_blank_ = true;
    if (!tkn_value_.empty() && curr_state_->acceptor.has_value()) {
      tokens_.push_back({curr_state_->acceptor.value(), tkn_value_});
      reset();
      return true;
    }
    return false;
  }

  is_blank_ = false;
  for (auto s : curr_state_->next) {
    if (match_state(chr, s)) return true;
  }
  return match(' ') && match(chr);
}

std::list<Token> Lexer::get() {
  return tokens_;
}

std::ostream& operator<<(std::ostream& os, const Lexer& matcher) {
  for (const auto& token : matcher.tokens_) {
    os << tt_to_str(token.type) << ", " << token.value << '\n';
  }
  return os;
}

bool Lexer::match_state(char chr, std::shared_ptr<State> st) {
  bool is_type_valid = match_state_type(chr, *st);
  switch (st->q) {
    case StateQuantifier::EO:
      if (!is_type_valid) return false;
      tkn_value_ += chr;
      curr_state_ = st;
      return true;
    case StateQuantifier::ZO:
      if (is_type_valid) tkn_value_ += chr;
      curr_state_ = st;
      return match(chr);
    case StateQuantifier::ZM:
      if (is_type_valid) {
        tkn_value_ += chr;
        return true;
      } else {
        curr_state_ = st;
        return match(chr);
      }
    default:
      return false;
  }
}

bool Lexer::match_state_type(char ch, const State& st) {
  switch (st.t) {
    case StateType::ELEMT:
      return ch == st.c;
    case StateType::WCARD:
      return true;
    case StateType::ALPHA:
      return std::isalpha(ch);
    case StateType::DIGIT:
      return std::isdigit(ch);
    default:
      return false;
  }
}

void Lexer::reset() {
  curr_state_ = tree_.root_;
  tkn_value_ = "";
  is_blank_ = true;
}

}  // namespace lexer
}  // namespace kuel
