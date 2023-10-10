#include "interpreter.h"

Interpreter::Interpreter()
    : lexer_(new Lexer),
      parser_(new Parser(lexer_->TokenStream(), Parser::Mode::INTERACTIVE)) {}

void Interpreter::Interpret(std::string source) {
  // Debug printing.
  for (const auto& token : Lex(source)) std::cout << token;
  std::cout << "\n";

  lexer_->SetSource(std::move(source));
  // TODO(erik): Clear parser?

  std::cout << parser_->Parse();
}