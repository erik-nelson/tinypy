#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

struct Token {
  enum class Type {
    // Indentation.
    INDENT,
    DEDENT,
    NEWLINE,

    // Keywords.
    AND,
    AS,
    ASSERT,
    ASYNC,
    AWAIT,
    BREAK,
    CLASS,
    CONTINUE,
    DEF,
    DEL,
    ELIF,
    ELSE,
    EXCEPT,
    FALSE,
    FINALLY,
    FOR,
    FROM,
    GLOBAL,
    IF,
    IMPORT,
    IN,
    IS,
    LAMBDA,
    NONE,
    NONLOCAL,
    NOT,
    OR,
    PASS,
    RAISE,
    RETURN,
    TRUE,
    TRY,
    WHILE,
    WITH,
    YIELD,

    // Identifiers.
    IDENTIFIER,

    // Literals.
    INTEGER,
    FLOAT,
    STRING,
    BOOLEAN,

    // Operators.
    PLUS,            // +
    MINUS,           // -
    MULTIPLY,        // *
    POWER,           // **
    DIVIDE,          // /
    FLOOR_DIVIDE,    // //
    MODULO,          // %
    AT,              // @
    LEFT_SHIFT,      // <<
    RIGHT_SHIFT,     // >>
    BITWISE_AND,     // &
    BITWISE_OR,      // |
    BITWISE_XOR,     // ^
    BITWISE_NOT,     // ~
    EXPR_ASSIGN,     // :=
    LESS_THAN,       // <
    GREATER_THAN,    // >
    LESS_EQUAL,      // <=
    GREATER_EQUAL,   // >=
    EQUALS,          // ==
    NOT_EQUALS,      // !=

    // Delimiters.
    // TODO(erik): Handle decorators ("@"). Overloaded against "AT" operator.
    LEFT_PAREN,      // (
    RIGHT_PAREN,     // )
    LEFT_BRACKET,    // [
    RIGHT_BRACKET,   // ]
    LEFT_BRACE,      // {
    RIGHT_BRACE,     // }
    COMMA,           // ,
    COLON,           // :
    MEMBER_ACCESS,   // .
    SEMICOLON,       // ;
    // DECORATE,        // @
    ASSIGN,          // =
    ANNOTATE,        // ->
    PLUS_ASSIGN,     // +=
    MINUS_ASSIGN,    // -=
    MULTIPLY_ASSIGN, // *=
    DIVIDE_ASSIGN,   // /=
    FLOOR_DIVIDE_ASSIGN, // //=
    MODULO_ASSIGN,   // %=
    AT_ASSIGN,       // @=
    AND_ASSIGN,      // &=
    OR_ASSIGN,       // |=
    XOR_ASSIGN,      // ^=
    RIGHT_SHIFT_ASSIGN, // >>=
    LEFT_SHIFT_ASSIGN,  // <<=
    POWER_ASSIGN,    // **=
  };

  // Start/end values for iterating over subtypes of tokens.
  static constexpr size_t kIndentationBegin = static_cast<size_t>(Type::INDENT);
  static constexpr size_t kIndentationEnd = static_cast<size_t>(Type::NEWLINE);
  static constexpr size_t kKeywordBegin = static_cast<size_t>(Type::AND);
  static constexpr size_t kKeywordEnd = static_cast<size_t>(Type::YIELD);
  static constexpr size_t kIdentifierBegin = static_cast<size_t>(Type::IDENTIFIER);
  static constexpr size_t kIdentifierEnd = static_cast<size_t>(Type::IDENTIFIER);
  static constexpr size_t kLiteralBegin = static_cast<size_t>(Type::INTEGER);
  static constexpr size_t kLiteralEnd = static_cast<size_t>(Type::BOOLEAN);
  static constexpr size_t kOperatorBegin = static_cast<size_t>(Type::PLUS);
  static constexpr size_t kOperatorEnd = static_cast<size_t>(Type::NOT_EQUALS);
  static constexpr size_t kDelimiterBegin = static_cast<size_t>(Type::LEFT_PAREN);
  static constexpr size_t kDelimiterEnd = static_cast<size_t>(Type::POWER_ASSIGN);
    
  // The type of token.
  Type type;

  // The token's value. Populated for literals and identifiers.
  std::optional<std::string> value;

  // The token string.
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
  void Print() const;
};

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