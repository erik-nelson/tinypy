#pragma once

#include "stream.h"
#include "token.h"

class Parser {
 public:
  explicit Parser(StreamReader<Token> reader);

 private:
  StreamReader<Token> reader_;
};