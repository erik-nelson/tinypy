#include "token.h"

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

void Token::Print() const {
    printf("Token: type = \'%s\'", String().data());
    if (value) printf(", value = \'%s\'", value->c_str());
    printf("\n");
}


 const std::unordered_map<std::string, Token::Type> kTokenStringToType = 
  []() -> std::unordered_map<std::string, Token::Type> {
    std::unordered_map<std::string, Token::Type> tokens;

    // Indentation.
    tokens["\n"] = Token::Type::NEWLINE;

    // Keywords.
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
    tokens["lambda"] = Token::Type::LAMBDA;
    tokens["none"] = Token::Type::NONE;                
    tokens["nonlocal"] = Token::Type::NONLOCAL;
    tokens["not"] = Token::Type::NOT;
    tokens["or"] = Token::Type::OR;
    tokens["pass"] = Token::Type::PASS;
    tokens["raise"] = Token::Type::RAISE;
    tokens["return"] = Token::Type::RETURN;
    tokens["True"] = Token::Type::TRUE;
    tokens["try"] = Token::Type::TRY;
    tokens["while"] = Token::Type::WHILE;
    tokens["with"] = Token::Type::WITH;
    tokens["yield"] = Token::Type::YIELD;

    // Literals.
    // TODO(erik)

    // Operators.
    tokens["+"] = Token::Type::PLUS;
    tokens["-"] = Token::Type::MINUS;
    tokens["*"] = Token::Type::MULTIPLY;
    tokens["**"] = Token::Type::POWER;
    tokens["/"] = Token::Type::DIVIDE;
    tokens["//"] = Token::Type::FLOOR_DIVIDE;
    tokens["%"] = Token::Type::MODULO;
    tokens["@"] = Token::Type::AT;
    tokens["<<"] = Token::Type::LEFT_SHIFT;
    tokens[">>"] = Token::Type::RIGHT_SHIFT;
    tokens["&"] = Token::Type::BITWISE_AND;
    tokens["|"] = Token::Type::BITWISE_OR;
    tokens["^"] = Token::Type::BITWISE_XOR;
    tokens["~"] = Token::Type::BITWISE_NOT;
    tokens[":="] = Token::Type::EXPR_ASSIGN;
    tokens["<"] = Token::Type::LESS_THAN;
    tokens[">"] = Token::Type::GREATER_THAN;
    tokens["<="] = Token::Type::LESS_EQUAL;
    tokens[">="] = Token::Type::GREATER_EQUAL;
    tokens["=="] = Token::Type::EQUALS;
    tokens["!="] = Token::Type::NOT_EQUALS;

    // Delimiters.
    tokens["("] = Token::Type::LEFT_PAREN;
    tokens[")"] = Token::Type::RIGHT_PAREN;
    tokens["["] = Token::Type::LEFT_BRACKET;
    tokens["]"] = Token::Type::RIGHT_BRACKET;
    tokens["{"] = Token::Type::LEFT_BRACE;
    tokens["}"] = Token::Type::RIGHT_BRACE;
    tokens[","] = Token::Type::COMMA;
    tokens[":"] = Token::Type::COLON;
    tokens["."] = Token::Type::MEMBER_ACCESS;
    tokens[";"] = Token::Type::SEMICOLON;
    tokens["="] = Token::Type::ASSIGN;
    tokens["->"] = Token::Type::ANNOTATE;
    tokens["+="] = Token::Type::PLUS_ASSIGN;
    tokens["-="] = Token::Type::MINUS_ASSIGN;
    tokens["*="] = Token::Type::MULTIPLY_ASSIGN;
    tokens["/="] = Token::Type::DIVIDE_ASSIGN;
    tokens["//="] = Token::Type::FLOOR_DIVIDE_ASSIGN;
    tokens["%="] = Token::Type::MODULO_ASSIGN;
    tokens["@="] = Token::Type::AT_ASSIGN;
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
    std::unordered_map<Token::Type, std::string> type_to_string;
    for (const auto& [string, type] : kTokenStringToType) {
        type_to_string[type] = string;
    }

    // Indentation.
    type_to_string[Token::Type::INDENT] = "indent";
    type_to_string[Token::Type::DEDENT] = "dedent";

    // Identifiers.
    type_to_string[Token::Type::IDENTIFIER] = "identifier";

    // Literals.
    type_to_string[Token::Type::INTEGER] = "int";
    type_to_string[Token::Type::FLOAT] = "float";
    type_to_string[Token::Type::STRING] = "str";
    type_to_string[Token::Type::BOOLEAN] = "bool";

    return type_to_string;
  }();

bool IsIndentation(Token::Type type) {
  return static_cast<size_t>(type) >= Token::kIndentationBegin 
         && static_cast<size_t>(type) <= Token::kIndentationEnd;
}

bool IsKeyword(Token::Type type) {
  return static_cast<size_t>(type) >= Token::kKeywordBegin 
         && static_cast<size_t>(type) <= Token::kKeywordEnd;
}

bool IsIdentifier(Token::Type type) {
  return static_cast<size_t>(type) >= Token::kIdentifierBegin 
         && static_cast<size_t>(type) <= Token::kIdentifierEnd;
}

bool IsLiteral(Token::Type type) {
  return static_cast<size_t>(type) >= Token::kLiteralBegin 
         && static_cast<size_t>(type) <= Token::kLiteralEnd;
}

bool IsOperator(Token::Type type) {
  return static_cast<size_t>(type) >= Token::kOperatorBegin 
         && static_cast<size_t>(type) <= Token::kOperatorEnd;
}

bool IsDelimiter(Token::Type type) {
  return static_cast<size_t>(type) >= Token::kDelimiterBegin 
         && static_cast<size_t>(type) <= Token::kDelimiterEnd;
}