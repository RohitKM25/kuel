#pragma once

#include <kuel/parser.hpp>

namespace kuel {

class CodeGenerator {
 public:
  CodeGenerator(parser::ASTPrg prg) : prg_(prg) {}
  std::string gen_stmt(parser::ASTStmt& stmt);
  std::string generate();

 private:
  parser::ASTPrg prg_;
};

}  // namespace kuel
