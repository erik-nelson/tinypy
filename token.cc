#include "token.h"

Token::Token(Type type, std::optional<std::string> value)
    : type(type), value(std::move(value)) {}

std::string_view Token::String() const {
  return {kTokenTypeToString.at(type).data()};
}

size_t Token::Length() const { return String().size(); }
bool Token::IsIndentation() const { return ::IsIndentation(type); }
bool Token::IsKeyword() const { return ::IsKeyword(type); }
bool Token::IsIdentifier() const { return ::IsIdentifier(type); }
bool Token::IsLiteral() const { return ::IsLiteral(type); }
bool Token::IsOperator() const { return ::IsOperator(type); }
bool Token::IsDelimiter() const { return ::IsDelimiter(type); }

std::string Token::DebugString() const {
  std::string debug_string;
  debug_string += "Token: type = \'" + std::string(String()) + "\'";
  if (value) debug_string += ", value = \'" + *value + "\'";
  debug_string += "\n";
  return debug_string;
}

bool Token::operator==(const Token& rhs) const {
  return type == rhs.type && value == rhs.value;
}

bool Token::operator!=(const Token& rhs) const { return !(*this == rhs); }

std::ostream& operator<<(std::ostream& os, const Token& token) {
  os << token.DebugString();
  return os;
}

const std::unordered_map<std::string, Token::Type> kTokenStringToType =
    []() -> std::unordered_map<std::string, Token::Type> {
  std::unordered_map<std::string, Token::Type> tokens;
  tokens["\n"] = Token::Type::NEWLINE;
  tokens["and"] = Token::Type::AND;
  tokens["as"] = Token::Type::AS;
  tokens["assert"] = Token::Type::ASSERT;
  tokens["async"] = Token::Type::ASYNC;
  tokens["await"] = Token::Type::AWAIT;
  tokens["break"] = Token::Type::BREAK;
  tokens["class"] = Token::Type::CLASS;
  tokens["continue"] = Token::Type::CONTINUE;
  tokens["def"] = Token::Type::DEF;
  tokens["del"] = Token::Type::DEL;
  tokens["elif"] = Token::Type::ELIF;
  tokens["else"] = Token::Type::ELSE;
  tokens["except"] = Token::Type::EXCEPT;
  tokens["False"] = Token::Type::FALSE;
  tokens["finally"] = Token::Type::FINALLY;
  tokens["for"] = Token::Type::FOR;
  tokens["from"] = Token::Type::FROM;
  tokens["global"] = Token::Type::GLOBAL;
  tokens["if"] = Token::Type::IF;
  tokens["import"] = Token::Type::IMPORT;
  tokens["in"] = Token::Type::IN;
  tokens["is"] = Token::Type::IS;
  tokens["is not"] = Token::Type::IS_NOT;
  tokens["lambda"] = Token::Type::LAMBDA;
  tokens["None"] = Token::Type::NONE;
  tokens["nonlocal"] = Token::Type::NONLOCAL;
  tokens["not"] = Token::Type::NOT;
  tokens["not in"] = Token::Type::NOT_IN;
  tokens["or"] = Token::Type::OR;
  tokens["pass"] = Token::Type::PASS;
  tokens["raise"] = Token::Type::RAISE;
  tokens["return"] = Token::Type::RETURN;
  tokens["True"] = Token::Type::TRUE;
  tokens["try"] = Token::Type::TRY;
  tokens["while"] = Token::Type::WHILE;
  tokens["with"] = Token::Type::WITH;
  tokens["yield"] = Token::Type::YIELD;
  tokens["+"] = Token::Type::PLUS;
  tokens["-"] = Token::Type::MINUS;
  tokens["*"] = Token::Type::MULTIPLY;
  tokens["**"] = Token::Type::POWER;
  tokens["/"] = Token::Type::DIVIDE;
  tokens["//"] = Token::Type::FLOOR_DIVIDE;
  tokens["%"] = Token::Type::MODULO;
  tokens["@"] = Token::Type::MATMUL;
  tokens["<<"] = Token::Type::LEFT_SHIFT;
  tokens[">>"] = Token::Type::RIGHT_SHIFT;
  tokens["&"] = Token::Type::BITWISE_AND;
  tokens["|"] = Token::Type::BITWISE_OR;
  tokens["^"] = Token::Type::BITWISE_XOR;
  tokens["~"] = Token::Type::INVERT;
  tokens[":="] = Token::Type::NAMED_EXPR;
  tokens["<"] = Token::Type::LESS_THAN;
  tokens[">"] = Token::Type::GREATER_THAN;
  tokens["<="] = Token::Type::LESS_EQUAL;
  tokens[">="] = Token::Type::GREATER_EQUAL;
  tokens["=="] = Token::Type::EQUALS;
  tokens["!="] = Token::Type::NOT_EQUALS;
  tokens["("] = Token::Type::LEFT_PAREN;
  tokens[")"] = Token::Type::RIGHT_PAREN;
  tokens["["] = Token::Type::LEFT_BRACKET;
  tokens["]"] = Token::Type::RIGHT_BRACKET;
  tokens["{"] = Token::Type::LEFT_BRACE;
  tokens["}"] = Token::Type::RIGHT_BRACE;
  tokens[","] = Token::Type::COMMA;
  tokens[":"] = Token::Type::COLON;
  tokens["."] = Token::Type::ATTRIBUTE;
  tokens[";"] = Token::Type::SEMICOLON;
  tokens["="] = Token::Type::ASSIGN;
  tokens["->"] = Token::Type::ANNOTATE;
  tokens["+="] = Token::Type::PLUS_ASSIGN;
  tokens["-="] = Token::Type::MINUS_ASSIGN;
  tokens["*="] = Token::Type::MULTIPLY_ASSIGN;
  tokens["/="] = Token::Type::DIVIDE_ASSIGN;
  tokens["//="] = Token::Type::FLOOR_DIVIDE_ASSIGN;
  tokens["%="] = Token::Type::MODULO_ASSIGN;
  tokens["@="] = Token::Type::MATMUL_ASSIGN;
  tokens["&="] = Token::Type::AND_ASSIGN;
  tokens["|="] = Token::Type::OR_ASSIGN;
  tokens["^="] = Token::Type::XOR_ASSIGN;
  tokens[">>="] = Token::Type::RIGHT_SHIFT_ASSIGN;
  tokens["<<="] = Token::Type::LEFT_SHIFT_ASSIGN;
  tokens["**="] = Token::Type::POWER_ASSIGN;

  return tokens;
}();

const std::unordered_map<Token::Type, std::string> kTokenTypeToString =
    []() -> std::unordered_map<Token::Type, std::string> {
  std::unordered_map<Token::Type, std::string> tokens;
  for (const auto& [string, type] : kTokenStringToType) tokens[type] = string;
  // Add token types that have no corresponding string.
  tokens[Token::Type::INDENT] = "@idt";
  tokens[Token::Type::DEDENT] = "@ddt";
  tokens[Token::Type::NEWLINE] = "@eol";
  tokens[Token::Type::IDENTIFIER] = "@nam";
  tokens[Token::Type::INTEGER] = "@int";
  tokens[Token::Type::FLOAT] = "@flt";
  tokens[Token::Type::STRING] = "@str";
  return tokens;
}();

bool IsIndentation(Token::Type type) {
  return static_cast<size_t>(type) >= Token::kIndentationBegin &&
         static_cast<size_t>(type) <= Token::kIndentationEnd;
}

bool IsKeyword(Token::Type type) {
  return static_cast<size_t>(type) >= Token::kKeywordBegin &&
         static_cast<size_t>(type) <= Token::kKeywordEnd;
}

bool IsIdentifier(Token::Type type) {
  return static_cast<size_t>(type) >= Token::kIdentifierBegin &&
         static_cast<size_t>(type) <= Token::kIdentifierEnd;
}

bool IsLiteral(Token::Type type) {
  return static_cast<size_t>(type) >= Token::kLiteralBegin &&
         static_cast<size_t>(type) <= Token::kLiteralEnd;
}

bool IsOperator(Token::Type type) {
  return static_cast<size_t>(type) >= Token::kOperatorBegin &&
         static_cast<size_t>(type) <= Token::kOperatorEnd;
}

bool IsDelimiter(Token::Type type) {
  return static_cast<size_t>(type) >= Token::kDelimiterBegin &&
         static_cast<size_t>(type) <= Token::kDelimiterEnd;
}