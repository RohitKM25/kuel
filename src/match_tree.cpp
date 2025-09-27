#include <kuel/lexer.hpp>

namespace kuel {

namespace lexer {

MatchTree::MatchTree() {
  root_ = std::make_shared<State>();
}

void MatchTree::append(const std::string& src, TokenType type) {
  parse(src, 0, type, *root_);
}

void MatchTree::parse(const std::string& src, size_t idx, TokenType type,
                      State& curr_state) {
  const char curr_c = src[idx];

  switch (curr_c) {
    case '+': {
      curr_state.next.push_back(std::make_shared<State>(
          curr_c, StateType::DIGIT, StateQuantifier::ZM));
      curr_state.q = StateQuantifier::EO;
      if (idx == src.size() - 1) curr_state.acceptor.emplace(type);
      break;
    }
    case '*': {
      curr_state.q = StateQuantifier::ZM;
      break;
    }
    case '?': {
      curr_state.q = StateQuantifier::ZO;
      break;
    }
    case '\\': {
      switch (src[++idx]) {
        case 'd': {
          curr_state.next.push_back(std::make_shared<State>(
              'd', StateType::DIGIT, StateQuantifier::EO));
          break;
        }
        default:
          break;
      }
      break;
    }
    default: {
      curr_state.next.push_back(std::make_shared<State>(
          curr_c, StateType::ELEMT, StateQuantifier::EO));
      break;
    }
  }

  if (static_cast<int>(idx) >= static_cast<int>(src.size()) - 1) {
    (curr_state.next.empty() ? curr_state : *curr_state.next.back())
        .acceptor.emplace(type);
  } else
    parse(src, idx + 1, type,
          curr_state.next.empty() ? curr_state : *curr_state.next.back());
}

std::ostream& operator<<(std::ostream& os, const MatchTree& m) {
  os << *m.root_;
  return os;
}

}  // namespace lexer
}  // namespace kuel
