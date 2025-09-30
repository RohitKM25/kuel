#pragma once

#include <kuel/lexer.hpp>
#include <list>
#include <variant>

namespace kuel {

namespace parser {

struct ASTIntLtr {
  int value;
};

struct ASTExitStmt {
  ASTIntLtr exit_code;
};

struct ASTStmt {
  std::variant<ASTExitStmt> stmt;
};

struct ASTPrg {
  std::list<ASTStmt> stmts;
};

class Parser {
 public:
  Parser(std::list<lexer::Token> tokens);
  std::optional<ASTPrg> get();

 private:
  std::list<lexer::Token> tokens_;
  ASTPrg tree_;

  bool parse_exit_stmt();
  bool run();
};

}  // namespace parser
}  // namespace kuel
