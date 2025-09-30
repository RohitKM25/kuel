#include <kuel/code_generator.hpp>
#include <sstream>

namespace kuel {

std::string CodeGenerator::gen_stmt(parser::ASTStmt& stmt) {
  struct Gate {
    std::string value;
    void operator()(parser::ASTExitStmt& s) {
      std::stringstream ss;
      ss << "\tmov rax, 60\n";
      ss << "\tmov rdi, " << s.exit_code.value << "\n";
      ss << "\tsyscall\n";
      value = ss.str();
    }
  } g;

  std::visit(g, stmt.stmt);

  return g.value;
}

std::string CodeGenerator::generate() {
  std::stringstream ss;
  ss << "\tbits 64\n";
  ss << "\tsection .text\n";
  ss << "\tglobal _start\n\n";
  ss << "_start:\n";
  for (auto& stmt : prg_.stmts) {
    ss << gen_stmt(stmt);
  }
  ss << "\n";
  return ss.str();
}

}  // namespace kuel
