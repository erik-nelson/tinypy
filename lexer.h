#pragma once

#include <deque>
#include <functional>
#include <optional>
#include <string>
#include <utility>

#include "token.h"

// Lexes a given set of source lines into tokens, following
// https://docs.python.org/3/reference/lexical_analysis.html
class Lexer {
 public:
  Lexer() = default;
  explicit Lexer(std::string source);

  void SetSource(std::string source);
  std::optional<Token> NextToken();

 private:
  // Whether we have any tokens available.
  bool HaveTokens() const { return !tokens_.empty(); }

  bool KeepGoing() const { return idx_ < source_.size(); }

  // Increment `idx_`, eating the next character available in the provided 
  // `source_` code. Returns false when we have reached the end of `source_`.
  bool EatChar();

  // Increment `idx_`, eating whitespace at the front of `source_`. Returns
  // false when we have reached the end of `source_`.
  bool EatWhitespace();

  // Helper that increments `idx_` until the predicate is met. Returns false 
  // when we have reached the end of `source_`.
  bool EatUntil(std::function<bool(size_t)> predicate);

  // TODO(erik): Describe.
  bool MatchIndentation();
  bool MatchKeyword();
  bool MatchOperatorOrDelimiter();
  bool MatchLiteral();
  bool MatchIdentifier();

  // Position within `source_`.
  size_t idx_ = 0u;

  // Current indentation level, in number of tab widths.
  int indentation_ = 0;
  
  // Raw source code.
  std::string source_;

  // Tokens already processed on the previous call to NextToken(). Consumed before
  // continuing.
  std::deque<Token> tokens_;
};