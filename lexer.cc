#include "lexer.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <regex>
#include <vector>
#include <utility>

namespace {
// Hard coded indentation width.
static constexpr size_t kIndentationWidth = 4u;

// Regex that captures python strings (combos of single, double, triple quitos, 
// with optional leading 'f', 'r', 'u', 'b'). e.g. 'text', "text", '''text''', 
// r'text\\n more text'.
static const std::regex kStringLiteralRegex("^(r|u|R|U|b|B|f|F)?((?:'''[^']*'''|\"\"\"[^']*\"\"\"|'[^'\\\\]*(\\\\.[^'\\\\]*)*'|\"[^\"\\\\]*(\\\\.[^\"\\\\]*)*\"))");

// Regex that captures python integers. e.g. 42, -123, 0x1A, 0b1101.
static const std::regex kIntLiteralRegex("^([-+]?\\b(0[xX][0-9A-Fa-f]+|0[bB][01]+|[1-9][0-9]*|0)\\b)");

// Regex that captures python floats. e.g. 3.14159, -0.12345, 1e5, -2.5e-3.
static const std::regex kFloatLiteralRegex("^([-+]?\\b\\d+\\.\\d*(?:[eE][-+]?\\d+)?\\b)");

// Regex that captures valid python identifiers (class names, function names, variable names).
// Names must start with a character in a-z, A-Z, or _. Valid variable names then continue with
// a-z, A-Z, 0-9, or additional underscores. The \b ensures standalone word names. 
// e.g. 'abc123', 'Abc123', 'aBc123', '_abc123', 'abc_123'. 
static const std::regex kIdentifierRegex("^(\\b[a-zA-Z_][a-zA-Z0-9_]*\\b)");

// Return whether `source` starts with `token` beginning at `idx`.
template <typename U, typename V>
bool MatchToken(const U& source, size_t idx, const V& token) {
  return source.compare(idx, token.size(), token) == 0;
}

// Given a list of candidate tokens that were lexed from the source code, return the best match. Matches
// are determined by length, e.g. if the source code reads "class ", matches would include {"as", "class"},
// in which case "class" is chosen as the desired token.
Token& BestMatch(std::vector<Token>& tokens) {
  return *std::max_element(tokens.begin(), tokens.end(), 
            [](const Token& lhs, const Token& rhs) { 
              return lhs.Length() < rhs.Length(); 
            });
} 
} // namespace


Lexer::Lexer(std::string source) { SetSource(std::move(source)); }

void Lexer::SetSource(std::string source) {
  idx_ = 0u;
  indentation_ = 0;
  source_ = std::move(source);
  tokens_.clear();
}

std::optional<Token> Lexer::NextToken() {
  // Eat chars until we lex more tokens.
  while (!HaveTokens() && EatChar());

  // Return the next token if available.
  if (HaveTokens()) {
    Token token = std::move(tokens_.front());
    tokens_.pop_front();
    return token;
  }

  // No more tokens available; we've exhausted the source code.
  return std::nullopt;
}

bool Lexer::EatChar() {
  if (!KeepGoing()) return false;
 
  // Try to find indentation related tokens.
  if (MatchIndentation()) return KeepGoing();

  // Try to find keyword tokens.
  if (MatchKeyword()) return KeepGoing();

  // Try to find literal tokens.
  if (MatchLiteral()) return KeepGoing();

  // Try to find operator or delimiter tokens.
  if (MatchOperatorOrDelimiter()) return KeepGoing();

  // Try to find identifier tokens.
  if (MatchIdentifier()) return KeepGoing();

  ++idx_;
  return KeepGoing();  // Skip white space implicitly.
}

bool Lexer::EatUntil(std::function<bool(size_t)> predicate) {
  while (idx_ < source_.size()) {
    if (predicate(idx_)) break;
    ++idx_;
  }
  return idx_ < source_.size();
}

bool Lexer::MatchIndentation() {
  bool matched = false;
  bool eat_indentation = (idx_ == 0);

  // Check for newlines. Repeated newlines are interpreted as a single newline.
  while (idx_ < source_.size() && source_[idx_] == '\n') {
    if (!matched) {
      tokens_.emplace_back(Token::Type::NEWLINE);
      eat_indentation = true;
      matched = true;
    }
    ++idx_;
  }

  // Consume indentation from the beginning of a line.
  if (eat_indentation) {
    size_t whitespace = 0u;

    while (idx_ < source_.size()) {
      if (source_[idx_] == ' ') whitespace += 1;
      else if (source_[idx_] == '\t') whitespace += kIndentationWidth;
      else break;
      ++idx_;
    }

    // Check for errors in indentation level.
    if (whitespace % kIndentationWidth != 0) {
      // TODO(erik): Improve error messages - add amount of whitespace.
      throw std::runtime_error("Encountered unexpected indentation");
    }

    const int new_indentation = whitespace / kIndentationWidth;
    if (new_indentation < 0) {
      throw std::runtime_error("Encountered negative indentation");
    }

    const int delta_indentation = new_indentation - indentation_;
    indentation_ = new_indentation;
    if (delta_indentation > 1) {
      throw std::runtime_error("Encountered unexpected delta indentation (>1 level)");
    }

    // Insert new indent or dedent tokens.
    for (int i = 0; i < std::abs(delta_indentation); ++i) {
      const Token::Type type = (delta_indentation < 0 ? Token::Type::DEDENT : Token::Type::INDENT);
      tokens_.emplace_back(type);
      matched = true;
    }
  }

  return matched;
}

bool Lexer::MatchKeyword() {
  bool matched = false;

  // Find keyword tokens that match at the current source location.
  std::vector<Token> matched_tokens;
  size_t i = static_cast<size_t>(Token::kKeywordBegin);
  for (; i <= static_cast<size_t>(Token::kKeywordEnd); ++i) {
    const Token::Type type = static_cast<Token::Type>(i);
    const std::string& token = kTokenTypeToString.at(type);
    if (MatchToken(source_, idx_, token)) {
      // The token is only a match if it is not part of a larger word. For example,
      // we don't want to match the keyword `in` when given the substring `in_place_transpose`,
      // which should instead be an identifier.
      if (idx_ + token.size() < source_.size()) {
        const char next = source_[idx_ + token.size()];
        if (std::isalnum(next) || next == '_') continue;
      }
      matched_tokens.emplace_back(type);
    }
  }

  // If multiple tokens match, choose the best match.
  if (!matched_tokens.empty()) {
    Token& matched_token = BestMatch(matched_tokens);
    tokens_.emplace_back(matched_token);
    idx_ += tokens_.back().Length();
    matched = true;
  }

  return matched;
}

bool Lexer::MatchOperatorOrDelimiter() {
  bool matched = false;

  // Find operator tokens that match at the current source location.
  std::vector<Token> matched_tokens;
  size_t i = static_cast<size_t>(Token::kOperatorBegin);
  for (; i <= static_cast<size_t>(Token::kOperatorEnd); ++i) {
    const Token::Type type = static_cast<Token::Type>(i);
    const std::string& token = kTokenTypeToString.at(type);
    if (MatchToken(source_, idx_, token)) {
      matched_tokens.emplace_back(type);
    }
  }

  // Find delimiter tokens that match at the current source location.
  i = static_cast<size_t>(Token::kDelimiterBegin);
  for (; i <= static_cast<size_t>(Token::kDelimiterEnd); ++i) {
    const Token::Type type = static_cast<Token::Type>(i);
    const std::string& token = kTokenTypeToString.at(type);
    if (MatchToken(source_, idx_, token)) {
      matched_tokens.emplace_back(type);
    }
  }

  // If multiple tokens match, choose the best match.
  if (!matched_tokens.empty()) {
    Token& matched_token = BestMatch(matched_tokens);
    tokens_.emplace_back(matched_token);
    idx_ += tokens_.back().Length();
    matched = true;
  }

  return matched;
}

bool Lexer::MatchLiteral() {
  std::smatch match;
  std::string::const_iterator begin = source_.begin() + idx_;
  std::string::const_iterator end = source_.end();

  std::unordered_map<Token::Type, std::regex> regexes;
  regexes[Token::Type::STRING] = kStringLiteralRegex;
  regexes[Token::Type::INTEGER] = kIntLiteralRegex;
  regexes[Token::Type::FLOAT] = kFloatLiteralRegex;

  for (auto& [type, regex] : regexes) {
    if (std::regex_search(begin, end, match, regex)) {
      idx_ += match[0].length();

      Token token;
      token.type = type;
      token.value = match[0].str();
      tokens_.emplace_back(std::move(token));
      return true;
    }
  }

  // No match found.
  return false;
}

bool Lexer::MatchIdentifier() {
  // Search for any valid identifier.
  std::smatch match;
  std::string::const_iterator begin = source_.begin() + idx_;
  std::string::const_iterator end = source_.end();
  if (std::regex_search(begin, end, match, kIdentifierRegex)) {
      idx_ += match[0].length();

      Token token;
      token.type = Token::Type::IDENTIFIER;
      token.value = match[0].str();
      tokens_.emplace_back(std::move(token));
  }

  return !match.empty();
}

std::vector<Token> Lex(std::string source) {
  std::vector<Token> tokens;
  Lexer lexer(std::move(source));
  while (auto maybe_token = lexer.NextToken()) {
    tokens.emplace_back(std::move(*maybe_token));
  }
  return tokens;
}