#pragma once

#include <memory>
#include <string>

#include "lexer.h"
#include "parser.h"

class Interpreter {
 public:
  Interpreter();

  void Interpret(std::string source);

 private:
  std::unique_ptr<Lexer> lexer_;
  std::unique_ptr<Parser> parser_;
};