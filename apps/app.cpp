#include <cstdlib>
#include <fstream>
#include <iostream>
#include <kuel/lexer.hpp>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <variant>

using namespace kuel::lexer;

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
  Parser(std::list<Token> tokens) { tokens_ = tokens; }

  std::optional<ASTPrg> get() {
    if (run()) return tree_;
    return std::nullopt;
  }

 private:
  std::list<Token> tokens_;
  ASTPrg tree_;

  bool parse_exit_stmt() {
    if (tokens_.empty()) return false;
    tree_.stmts.push_back(
        {.stmt = ASTExitStmt{atoi(tokens_.front().value.c_str())}});
    tokens_.pop_front();
    if (tokens_.front().type != TokenType::EOL) return false;
    tokens_.pop_front();
    return true;
  }

  bool run() {
    while (!tokens_.empty()) {
      const auto& token = tokens_.front();
      tokens_.pop_front();
      switch (token.type) {
        case TokenType::KWD_EXIT:
          if (!parse_exit_stmt()) return false;
          break;
        case TokenType::SYB_BLKS:
          break;
        case TokenType::SYB_BLKE:
          break;
        default:
          break;
      }
    }
    return true;
  }
};

class CodeGenerator {
 public:
  CodeGenerator(ASTPrg prg) : prg_(prg) {}

  std::string gen_stmt(ASTStmt& stmt) {
    struct Gate {
      std::string value;
      void operator()(ASTExitStmt& s) {
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

  std::string generate() {
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

 private:
  ASTPrg prg_;
};

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Expected input file" << std::endl;
    return 1;
  }

  std::cout << "Creating Match Tree\n";

  std::unordered_map<std::string, TokenType> match_strs = {
      {"exit", TokenType::KWD_EXIT}, {"{", TokenType::SYB_BLKS},
      {"}", TokenType::SYB_BLKE},    {"\\d+", TokenType::LTR_INT},
      {"\n", TokenType::EOL},        {"\0", TokenType::EOL},
  };

  Lexer m(match_strs);

  std::ifstream src_file(argv[1]);

  std::cout << "Starting Lexical Analysis\n";

  char curr_ch;
  while ((curr_ch = src_file.get()) != src_file.eof()) {
    if (!m.match(curr_ch)) {
      break;
    }
  }

  auto tokens = m.get();

  std::cout << "Tokens generated.\n";

  // for (const auto& tkn : tokens)
  //   std::cout << tkn.value << ", " << tt_to_str(tkn.type) << '\n';

  Parser p(tokens);
  auto ast = p.get();

  if (!ast.has_value()) {
    std::cout << "Parsing Error\n";
    return 1;
  }
  std::cout << "Parsed AST.\n";

  CodeGenerator cg(ast.value());

  std::cout << "Generated Code\n";

  std::string output = cg.generate();

  {
    std::ofstream dst_asm_file("out.asm");
    dst_asm_file << output;
  }

  system("nasm -felf64 out.asm");
  system("ld -o out out.o");
  system("rm out.asm out.o");

  std::cout << std::flush;
  return 0;
}
