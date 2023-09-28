#include "interpreter.h"

void Interpreter::Interpret(std::string source) {
  lexer_.SetSource(std::move(source));

  while (auto maybe_token = lexer_.NextToken()) {
    maybe_token->Print();
  }
}