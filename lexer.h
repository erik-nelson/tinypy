#pragma once

#include <functional>
#include <optional>
#include <string>
#include <vector>
#include <utility>

#include "stream.h"
#include "token.h"

// Lexes a given set of source lines into tokens, following
// https://docs.python.org/3/reference/lexical_analysis.html
class Lexer {
 public:
  Lexer();
  explicit Lexer(std::string source);

  // Set the current source code to lex.
  void SetSource(std::string source);

  // Create a stream reader to read tokens from.
  // Example:
  // 
  //     std::string source_code = "a = 5 * 3 + 2";
  //     Lexer lexer(std::move(source_code));
  //     
  //     StreamReader<Token> stream = lexer.TokenStream();
  //     while (std::optional<Token> token = stream.Read()) {
  //       ...
  //     }
  //
  StreamReader<Token> TokenStream();

 private:
  // Whether we have any more source code available to lex.
  bool KeepGoing() const { return idx_ < source_.size(); }

  // Increment `idx_`, eating the next character available in the provided 
  // `source_` code. Populates the provided `buffer` with any new tokens encountered.
  // Returns false when we have reached the end of `source_`.
  bool EatChar(std::vector<Token>* buffer);

  // Attempt to match various language constituents from `source_` at the 
  // current `idx_`. Populates the provided `buffer` with any new tokens encountered.
  // Returns whether a match was found.
  bool MatchIndentation(std::vector<Token>* buffer);
  bool MatchKeyword(std::vector<Token>* buffer);
  bool MatchOperatorOrDelimiter(std::vector<Token>* buffer);
  bool MatchLiteral(std::vector<Token>* buffer);
  bool MatchIdentifier(std::vector<Token>* buffer);

  // Position within `source_`.
  size_t idx_ = 0u;

  // Current indentation level, in number of tab widths.
  int indentation_ = 0;
  
  // Raw source code.
  std::string source_;

  // Stream of tokens. Each EatChar() call adds an arbitrary number of new
  // tokens to the stream. Consumers pull from this stream.
  Stream<Token> tokens_;
};

// Standalone helper function that lexes the input source code to tokens in one call.
std::vector<Token> Lex(std::string source);