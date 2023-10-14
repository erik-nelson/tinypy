#include "interpreter.h"

Interpreter::Interpreter()
    : lexer_(new Lexer),
      parser_(new Parser(lexer_->TokenStream(), Parser::Mode::INTERACTIVE)) {}

void Interpreter::Interpret(std::string source) {
  {
    // Debug printing.
    // TODO(erik): Remove.
    for (const auto& token : Lex(source)) std::cout << token;
    std::cout << "\n";
  }

  lexer_->SetSource(std::move(source));
  parser_->Parse();
  std::cout << parser_->syntax_tree();
}