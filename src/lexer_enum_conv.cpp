#include <kuel/lexer.hpp>

namespace kuel {

namespace lexer {

const char* st_to_str(StateType t) {
  switch (t) {
    case StateType::ELEMT:
      return "ELEMT";
    case StateType::WCARD:
      return "WCARD";
    case StateType::ALPHA:
      return "ALPHA";
    case StateType::DIGIT:
      return "DIGIT";
    default:
      return "NIL";
  }
}

const char* sq_to_str(StateQuantifier q) {
  switch (q) {
    case StateQuantifier::EO:
      return "EO";
    case StateQuantifier::ZO:
      return "ZO";
    case StateQuantifier::OM:
      return "OM";
    case StateQuantifier::ZM:
      return "ZM";
    default:
      return "NIL";
  }
}

const char* tt_to_str(TokenType t) {
  switch (t) {
    case TokenType::KWD_EXIT:
      return "KWD_EXIT";
    case TokenType::LTR_INT:
      return "LTR_INT";
    case TokenType::EOL:
      return "EOL";
    default:
      return "NIL";
  }
}

std::ostream& operator<<(std::ostream& os, const State& s) {
  os << "(" << s.c << ", " << st_to_str(s.t) << ", " << sq_to_str(s.q);
  if (s.acceptor.has_value()) os << ", " << tt_to_str(s.acceptor.value());
  os << ") { ";
  for (const auto n : s.next) {
    os << *n << ", ";
  }
  os << "}";
  return os;
}

}  // namespace lexer
}  // namespace kuel
