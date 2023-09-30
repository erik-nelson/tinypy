#include "parser.h"

Parser::Parser(StreamReader<Token> reader) : reader_(std::move(reader)) {}
