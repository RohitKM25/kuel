#include <cstdlib>
#include <fstream>
#include <iostream>
#include <kuel.hpp>
#include <unordered_map>

using namespace kuel::lexer;

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

  kuel::parser::Parser p(tokens);
  auto ast = p.get();

  if (!ast.has_value()) {
    std::cout << "Parsing Error\n";
    return 1;
  }
  std::cout << "Parsed AST.\n";

  kuel::CodeGenerator cg(ast.value());

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
