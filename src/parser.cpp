#include <kuel/parser.hpp>

namespace kuel {

namespace parser {

Parser::Parser(std::list<lexer::Token> tokens) {
  tokens_ = tokens;
}

std::optional<ASTPrg> Parser::get() {
  if (run()) return tree_;
  return std::nullopt;
}

bool Parser::parse_exit_stmt() {
  if (tokens_.empty()) return false;
  tree_.stmts.push_back(
      {.stmt = ASTExitStmt{atoi(tokens_.front().value.c_str())}});
  tokens_.pop_front();
  if (tokens_.front().type != lexer::TokenType::EOL) return false;
  tokens_.pop_front();
  return true;
}

bool Parser::run() {
  while (!tokens_.empty()) {
    const auto& token = tokens_.front();
    tokens_.pop_front();
    switch (token.type) {
      case lexer::TokenType::KWD_EXIT:
        if (!parse_exit_stmt()) return false;
        break;
      case lexer::TokenType::SYB_BLKS:
        break;
      case lexer::TokenType::SYB_BLKE:
        break;
      default:
        break;
    }
  }
  return true;
}

}  // namespace parser
}  // namespace kuel
