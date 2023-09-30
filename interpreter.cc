#include "interpreter.h"

Interpreter::Interpreter() 
  : lexer_(new Lexer),
    parser_(new Parser(lexer_->TokenStream())) {}

void Interpreter::Interpret(std::string source) {
  lexer_->SetSource(std::move(source));
  // TODO(erik): Clear parser?

  StreamReader<Token> stream = lexer_->TokenStream();
  while (std::optional<Token> token = stream.Read()) {
    std::cout << *token;
  }
}