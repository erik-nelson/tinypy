#pragma once

#include <string>

#include "lexer.h"

class Interpreter {
 public:
  Interpreter() = default;

  void Interpret(std::string source);

 private:
  Lexer lexer_;
};