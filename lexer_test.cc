#include "lexer.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "token.h"

TEST(Lexer, Arithmetic) {
  const char* source = R"(result = 3 + 5 * 2)";
  std::cout << "Source:\n" << source << "\n";

  EXPECT_THAT(Lex(source), testing::ContainerEq(std::vector<Token>{
                               {Token::Type::IDENTIFIER, "result"},
                               {Token::Type::ASSIGN},
                               {Token::Type::INTEGER, "3"},
                               {Token::Type::PLUS},
                               {Token::Type::INTEGER, "5"},
                               {Token::Type::MULTIPLY},
                               {Token::Type::INTEGER, "2"},
                           }));
}

TEST(Lexer, FunctionDefinition) {
  const char* source = R"(
def add(a, b):
    return a + b
)";

  std::cout << "Source:\n" << source << "\n";

  EXPECT_THAT(Lex(source), testing::ContainerEq(std::vector<Token>{
                               {Token::Type::NEWLINE},
                               {Token::Type::DEF},
                               {Token::Type::IDENTIFIER, "add"},
                               {Token::Type::LEFT_PAREN},
                               {Token::Type::IDENTIFIER, "a"},
                               {Token::Type::COMMA},
                               {Token::Type::IDENTIFIER, "b"},
                               {Token::Type::RIGHT_PAREN},
                               {Token::Type::COLON},
                               {Token::Type::NEWLINE},
                               {Token::Type::INDENT},
                               {Token::Type::RETURN},
                               {Token::Type::IDENTIFIER, "a"},
                               {Token::Type::PLUS},
                               {Token::Type::IDENTIFIER, "b"},
                               {Token::Type::NEWLINE},
                               {Token::Type::DEDENT},
                           }));
}

TEST(Lexer, Literals) {
  const char* source = R"(
message = "Hello, World!"
my_list = [1, 2, 3]
)";

  std::cout << "Source:\n" << source << "\n";

  EXPECT_THAT(Lex(source), testing::ContainerEq(std::vector<Token>{
                               {Token::Type::NEWLINE},
                               {Token::Type::IDENTIFIER, "message"},
                               {Token::Type::ASSIGN},
                               {Token::Type::STRING, "\"Hello, World!\""},
                               {Token::Type::NEWLINE},
                               {Token::Type::IDENTIFIER, "my_list"},
                               {Token::Type::ASSIGN},
                               {Token::Type::LEFT_BRACKET},
                               {Token::Type::INTEGER, "1"},
                               {Token::Type::COMMA},
                               {Token::Type::INTEGER, "2"},
                               {Token::Type::COMMA},
                               {Token::Type::INTEGER, "3"},
                               {Token::Type::RIGHT_BRACKET},
                               {Token::Type::NEWLINE},
                           }));
}

TEST(Lexer, ControlFlow) {
  const char* source = R"(
if x > 10:
    print("x is greater than 10")
else:
    print("x is less than or equal to 10")
)";

  std::cout << "Source:\n" << source << "\n";

  EXPECT_THAT(Lex(source),
              testing::ContainerEq(std::vector<Token>{
                  {Token::Type::NEWLINE},
                  {Token::Type::IF},
                  {Token::Type::IDENTIFIER, "x"},
                  {Token::Type::GREATER_THAN},
                  {Token::Type::INTEGER, "10"},
                  {Token::Type::COLON},
                  {Token::Type::NEWLINE},
                  {Token::Type::INDENT},
                  {Token::Type::IDENTIFIER, "print"},
                  {Token::Type::LEFT_PAREN},
                  {Token::Type::STRING, "\"x is greater than 10\""},
                  {Token::Type::RIGHT_PAREN},
                  {Token::Type::NEWLINE},
                  {Token::Type::DEDENT},
                  {Token::Type::ELSE},
                  {Token::Type::COLON},
                  {Token::Type::NEWLINE},
                  {Token::Type::INDENT},
                  {Token::Type::IDENTIFIER, "print"},
                  {Token::Type::LEFT_PAREN},
                  {Token::Type::STRING, "\"x is less than or equal to 10\""},
                  {Token::Type::RIGHT_PAREN},
                  {Token::Type::NEWLINE},
                  {Token::Type::DEDENT},
              }));
}

TEST(Lexer, ClassDefinition) {
  const char* source = R"(
class Person:
    def __init__(self, name):
        self.name = name

    def greet(self):
        print(f"Hello, my name is {self.name}")

p = Person("Alice")
p.greet()
)";

  std::cout << "Source:\n" << source << "\n";
  EXPECT_THAT(Lex(source),
              testing::ContainerEq(std::vector<Token>{
                  {Token::Type::NEWLINE},
                  {Token::Type::CLASS},
                  {Token::Type::IDENTIFIER, "Person"},
                  {Token::Type::COLON},
                  {Token::Type::NEWLINE},
                  {Token::Type::INDENT},
                  {Token::Type::DEF},
                  {Token::Type::IDENTIFIER, "__init__"},
                  {Token::Type::LEFT_PAREN},
                  {Token::Type::IDENTIFIER, "self"},
                  {Token::Type::COMMA},
                  {Token::Type::IDENTIFIER, "name"},
                  {Token::Type::RIGHT_PAREN},
                  {Token::Type::COLON},
                  {Token::Type::NEWLINE},
                  {Token::Type::INDENT},
                  {Token::Type::IDENTIFIER, "self"},
                  {Token::Type::ATTRIBUTE},
                  {Token::Type::IDENTIFIER, "name"},
                  {Token::Type::ASSIGN},
                  {Token::Type::IDENTIFIER, "name"},
                  {Token::Type::NEWLINE},
                  {Token::Type::DEDENT},
                  {Token::Type::DEF},
                  {Token::Type::IDENTIFIER, "greet"},
                  {Token::Type::LEFT_PAREN},
                  {Token::Type::IDENTIFIER, "self"},
                  {Token::Type::RIGHT_PAREN},
                  {Token::Type::COLON},
                  {Token::Type::NEWLINE},
                  {Token::Type::INDENT},
                  {Token::Type::IDENTIFIER, "print"},
                  {Token::Type::LEFT_PAREN},
                  {Token::Type::STRING, "f\"Hello, my name is {self.name}\""},
                  {Token::Type::RIGHT_PAREN},
                  {Token::Type::NEWLINE},
                  {Token::Type::DEDENT},
                  {Token::Type::DEDENT},
                  {Token::Type::IDENTIFIER, "p"},
                  {Token::Type::ASSIGN},
                  {Token::Type::IDENTIFIER, "Person"},
                  {Token::Type::LEFT_PAREN},
                  {Token::Type::STRING, "\"Alice\""},
                  {Token::Type::RIGHT_PAREN},
                  {Token::Type::NEWLINE},
                  {Token::Type::IDENTIFIER, "p"},
                  {Token::Type::ATTRIBUTE},
                  {Token::Type::IDENTIFIER, "greet"},
                  {Token::Type::LEFT_PAREN},
                  {Token::Type::RIGHT_PAREN},
                  {Token::Type::NEWLINE},
              }));
}

TEST(Lexer, Exception) {
  const char* source = R"(
class CustomException(Exception):
    pass

try:
    value = int("not_an_integer")
except ValueError as e:
    raise CustomException("Invalid value") from e
)";

  std::cout << "Source:\n" << source << "\n";
  EXPECT_THAT(Lex(source), testing::ContainerEq(std::vector<Token>{
                               {Token::Type::NEWLINE},
                               {Token::Type::CLASS},
                               {Token::Type::IDENTIFIER, "CustomException"},
                               {Token::Type::LEFT_PAREN},
                               {Token::Type::IDENTIFIER, "Exception"},
                               {Token::Type::RIGHT_PAREN},
                               {Token::Type::COLON},
                               {Token::Type::NEWLINE},
                               {Token::Type::INDENT},
                               {Token::Type::PASS},
                               {Token::Type::NEWLINE},
                               {Token::Type::DEDENT},
                               {Token::Type::TRY},
                               {Token::Type::COLON},
                               {Token::Type::NEWLINE},
                               {Token::Type::INDENT},
                               {Token::Type::IDENTIFIER, "value"},
                               {Token::Type::ASSIGN},
                               {Token::Type::IDENTIFIER, "int"},
                               {Token::Type::LEFT_PAREN},
                               {Token::Type::STRING, "\"not_an_integer\""},
                               {Token::Type::RIGHT_PAREN},
                               {Token::Type::NEWLINE},
                               {Token::Type::DEDENT},
                               {Token::Type::EXCEPT},
                               {Token::Type::IDENTIFIER, "ValueError"},
                               {Token::Type::AS},
                               {Token::Type::IDENTIFIER, "e"},
                               {Token::Type::COLON},
                               {Token::Type::NEWLINE},
                               {Token::Type::INDENT},
                               {Token::Type::RAISE},
                               {Token::Type::IDENTIFIER, "CustomException"},
                               {Token::Type::LEFT_PAREN},
                               {Token::Type::STRING, "\"Invalid value\""},
                               {Token::Type::RIGHT_PAREN},
                               {Token::Type::FROM},
                               {Token::Type::IDENTIFIER, "e"},
                               {Token::Type::NEWLINE},
                               {Token::Type::DEDENT},
                           }));
}

TEST(Lexer, ListsAndLambdas) {
  const char* source = R"(
numbers = [1, 2, 3, 4, 5]
squared_numbers = [x ** 2 for x in numbers if x % 2 == 0]
double = lambda x: x * 2
result = double(10)
)";

  std::cout << "Source:\n" << source << "\n";
  EXPECT_THAT(Lex(source), testing::ContainerEq(std::vector<Token>{
                               {Token::Type::NEWLINE},
                               {Token::Type::IDENTIFIER, "numbers"},
                               {Token::Type::ASSIGN},
                               {Token::Type::LEFT_BRACKET},
                               {Token::Type::INTEGER, "1"},
                               {Token::Type::COMMA},
                               {Token::Type::INTEGER, "2"},
                               {Token::Type::COMMA},
                               {Token::Type::INTEGER, "3"},
                               {Token::Type::COMMA},
                               {Token::Type::INTEGER, "4"},
                               {Token::Type::COMMA},
                               {Token::Type::INTEGER, "5"},
                               {Token::Type::RIGHT_BRACKET},
                               {Token::Type::NEWLINE},
                               {Token::Type::IDENTIFIER, "squared_numbers"},
                               {Token::Type::ASSIGN},
                               {Token::Type::LEFT_BRACKET},
                               {Token::Type::IDENTIFIER, "x"},
                               {Token::Type::POWER},
                               {Token::Type::INTEGER, "2"},
                               {Token::Type::FOR},
                               {Token::Type::IDENTIFIER, "x"},
                               {Token::Type::IN},
                               {Token::Type::IDENTIFIER, "numbers"},
                               {Token::Type::IF},
                               {Token::Type::IDENTIFIER, "x"},
                               {Token::Type::MODULO},
                               {Token::Type::INTEGER, "2"},
                               {Token::Type::EQUALS},
                               {Token::Type::INTEGER, "0"},
                               {Token::Type::RIGHT_BRACKET},
                               {Token::Type::NEWLINE},
                               {Token::Type::IDENTIFIER, "double"},
                               {Token::Type::ASSIGN},
                               {Token::Type::LAMBDA},
                               {Token::Type::IDENTIFIER, "x"},
                               {Token::Type::COLON},
                               {Token::Type::IDENTIFIER, "x"},
                               {Token::Type::MULTIPLY},
                               {Token::Type::INTEGER, "2"},
                               {Token::Type::NEWLINE},
                               {Token::Type::IDENTIFIER, "result"},
                               {Token::Type::ASSIGN},
                               {Token::Type::IDENTIFIER, "double"},
                               {Token::Type::LEFT_PAREN},
                               {Token::Type::INTEGER, "10"},
                               {Token::Type::RIGHT_PAREN},
                               {Token::Type::NEWLINE},
                           }));
}