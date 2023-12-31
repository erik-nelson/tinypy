#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

struct Token {
  // TODO(erik): Handle soft keywords such as `match`, `case`, `_`:
  // https://docs.python.org/3/reference/lexical_analysis.html#soft-keywords
  enum class Type {
    // Indentation.
    INDENT,   // @idt
    DEDENT,   // @ddt
    NEWLINE,  // @eol

    // Keywords.
    AND,       // and
    AS,        // as
    ASSERT,    // assert
    ASYNC,     // async
    AWAIT,     // await
    BREAK,     // break
    CLASS,     // class
    CONTINUE,  // continue
    DEF,       // def
    DEL,       // del
    ELIF,      // elif
    ELSE,      // else
    EXCEPT,    // except
    FALSE,     // False
    FINALLY,   // finally
    FOR,       // for
    FROM,      // from
    GLOBAL,    // global
    IF,        // if
    IMPORT,    // import
    IN,        // in
    IS,        // is
    IS_NOT,    // is not
    LAMBDA,    // lambda
    NONE,      // None
    NONLOCAL,  // nonlocal
    NOT,       // not
    NOT_IN,    // not in
    OR,        // or
    PASS,      // pass
    RAISE,     // raise
    RETURN,    // return
    TRUE,      // True
    TRY,       // try
    WHILE,     // while
    WITH,      // with
    YIELD,     // yield

    // Identifiers.
    IDENTIFIER,  // @nam

    // Literals.
    INTEGER,  // @int
    FLOAT,    // @flt
    STRING,   // @str

    // Operators.
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    POWER,          // **
    DIVIDE,         // /
    FLOOR_DIVIDE,   // //
    MODULO,         // %
    MATMUL,         // @
    LEFT_SHIFT,     // <<
    RIGHT_SHIFT,    // >>
    BITWISE_AND,    // &
    BITWISE_OR,     // |
    BITWISE_XOR,    // ^
    INVERT,         // ~
    NAMED_EXPR,     // :=
    LESS_THAN,      // <
    GREATER_THAN,   // >
    LESS_EQUAL,     // <=
    GREATER_EQUAL,  // >=
    EQUALS,         // ==
    NOT_EQUALS,     // !=

    // Delimiters.
    LEFT_PAREN,           // (
    RIGHT_PAREN,          // )
    LEFT_BRACKET,         // [
    RIGHT_BRACKET,        // ]
    LEFT_BRACE,           // {
    RIGHT_BRACE,          // }
    COMMA,                // ,
    COLON,                // :
    ATTRIBUTE,            // .
    SEMICOLON,            // ;
    ASSIGN,               // =
    ANNOTATE,             // ->
    PLUS_ASSIGN,          // +=
    MINUS_ASSIGN,         // -=
    MULTIPLY_ASSIGN,      // *=
    DIVIDE_ASSIGN,        // /=
    FLOOR_DIVIDE_ASSIGN,  // //=
    MODULO_ASSIGN,        // %=
    MATMUL_ASSIGN,        // @=
    AND_ASSIGN,           // &=
    OR_ASSIGN,            // |=
    XOR_ASSIGN,           // ^=
    RIGHT_SHIFT_ASSIGN,   // >>=
    LEFT_SHIFT_ASSIGN,    // <<=
    POWER_ASSIGN,         // **=
    // TODO(erik): Handle decorators ("@"). Overloaded against "AT" operator.
    // DECORATE,         // @
  };

  // Start/end values for iterating over subtypes of tokens.
  static constexpr size_t kIndentationBegin = static_cast<size_t>(Type::INDENT);
  static constexpr size_t kIndentationEnd = static_cast<size_t>(Type::NEWLINE);
  static constexpr size_t kKeywordBegin = static_cast<size_t>(Type::AND);
  static constexpr size_t kKeywordEnd = static_cast<size_t>(Type::YIELD);
  static constexpr size_t kIdentifierBegin =
      static_cast<size_t>(Type::IDENTIFIER);
  static constexpr size_t kIdentifierEnd =
      static_cast<size_t>(Type::IDENTIFIER);
  static constexpr size_t kLiteralBegin = static_cast<size_t>(Type::INTEGER);
  static constexpr size_t kLiteralEnd = static_cast<size_t>(Type::STRING);
  static constexpr size_t kOperatorBegin = static_cast<size_t>(Type::PLUS);
  static constexpr size_t kOperatorEnd = static_cast<size_t>(Type::NOT_EQUALS);
  static constexpr size_t kDelimiterBegin =
      static_cast<size_t>(Type::LEFT_PAREN);
  static constexpr size_t kDelimiterEnd =
      static_cast<size_t>(Type::POWER_ASSIGN);

  // Constructors.
  Token() = default;
  Token(Type type, std::optional<std::string> value = std::nullopt);

  // The token string, e.g. an AWAIT token would return 'await'. For token
  // types that do not have an associated keyword, such as INDENT, DEDENT,
  // or IDENTIFIER, a placeholder debug string is used (e.g. 'indent').
  std::string_view String() const;

  // The token's length.
  size_t Length() const;

  // Helpers for token subtype.
  bool IsIndentation() const;
  bool IsKeyword() const;
  bool IsIdentifier() const;
  bool IsLiteral() const;
  bool IsOperator() const;
  bool IsDelimiter() const;

  // Debug printing.
  std::string DebugString() const;

  // Comparisons.
  bool operator==(const Token& rhs) const;
  bool operator!=(const Token& rhs) const;

  // The type of token.
  Type type;

  // The token's value. Populated for literals and identifiers.
  std::optional<std::string> value;
};

// Print token to ostream.
std::ostream& operator<<(std::ostream& os, const Token& token);

// Mapping from string to token type and vice versa.
extern const std::unordered_map<std::string, Token::Type> kTokenStringToType;
extern const std::unordered_map<Token::Type, std::string> kTokenTypeToString;

// Helpers for token subtype.
bool IsIndentation(Token::Type type);
bool IsKeyword(Token::Type type);
bool IsIdentifier(Token::Type type);
bool IsLiteral(Token::Type type);
bool IsOperator(Token::Type type);
bool IsDelimiter(Token::Type type);