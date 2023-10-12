#pragma once

#include <functional>
#include <stack>
#include <unordered_map>

#include "stream.h"
#include "syntax_tree.h"
#include "token.h"

// https://docs.python.org/3/reference/expressions.html#operator-precedence
enum class TokenPrecedence : int {
  NONE = 0,
  LAMBDA,           // lambda
  IF_EXP,           // x if y else z
  OR,               // or
  AND,              // and
  NOT,              // not
  COMPARISON,       // in, not in, is, is not, <, <=, >, >=, !=, ==
  BITWISE_OR,       // |
  BITWISE_XOR,      // ^
  BITWISE_AND,      // &
  BITWISE_SHIFT,    // <<, >>
  ADD_SUBTRACT,     // +, -
  MULTIPLY_DIVIDE,  // *, @, /, //, %
  BITWISE_NOT,      // ~
  POWER,            // **
  AWAIT,            // await
  CALL,             // x[i], x[a:b], x(...), x.a
  COMPREHENSION,    // (x,...), [x,...], {x:y,...}, {x,...}
};

// Pratt parsing rules, following:
// https://journal.stuffwithstuff.com/2011/03/19/pratt-parsers-expression-parsing-made-easy/
using ParseStatementRule = std::function<void()>;

// Our PrefixRule and InfixRule type aliases correspond to the PrefixParselet
// and InfixParselet concepts.
struct ParseExpressionRule {
  // A prefix parser eats a prefix token and applies it to the next expression
  // from the parser. E.g.
  //   source: `-(a + b)`
  //     - `-` is the prefix token
  //     - `(a + b)` is the next expression from the parser
  //
  using PrefixRule = std::function<void()>;
  // An infix parser eats an lhs node, and an infix token, and applies their
  // combination to the next expression from the parser. E.g.
  //  source: `a + b`
  //    - `a` is the lhs node
  //    - `+` is the infix token
  //    - `b` is the next expression from the parser
  //
  using InfixRule = std::function<void()>;

  PrefixRule prefix = nullptr;
  InfixRule infix = nullptr;
  TokenPrecedence precedence = TokenPrecedence::NONE;
};

class Parser {
 public:
  enum class Mode {
    MODULE,       // Parse from a python module or script.
    INTERACTIVE,  // Parse from a REPL or interactive session.
    EXPRESSION    // Parse a single expression.
  };

  explicit Parser(StreamReader<Token> tokens, Mode mode = Mode::MODULE);

  // TODO(erik): Describe.
  SyntaxTree& Parse() &;
  SyntaxTree&& Parse() &&;

 private:
  // The body of a block of python code is a series of statements.
  // Parse a single statement. Each statement potentially includes a set of
  // expressions.
  void ParseStatement();

  // Parse a single expression.
  void ParseExpression(TokenPrecedence precedence = TokenPrecedence::NONE);

  // Statements.
  // void ParseDefStatement();
  // void ParseAsyncStatement();
  // void ParseClassStatement();
  // void ParseReturnStatement();
  // void ParseDeleteStatement();
  // void ParseAssignStatement();
  // void ParseAugAssignStatement();
  // void ParseForStatement();
  // void ParseWhileStatement();
  // void ParseIfStatement();
  // void ParseWithStatement();
  // void ParseRaiseStatement();
  // void ParseTryStatement();
  // void ParseAssertStatement();
  // void ParseImportStatement();
  // void ParseGlobalStatement();
  // void ParseNonlocalStatement();
  // void ParsePassStatement();
  // void ParseBreakStatement();
  // void ParseContinueStatement();

  // Expressions.
  // void ParseBooleanOpExpression();
  // void ParseNamedExpression();
  void ParseUnaryOpExpression();
  void ParseBinaryOpExpression();
  // void ParseLambdaExpression();
  // void ParseIfExpression();
  // void ParseListExpression();
  // void ParseDictExpression();
  // void ParseSetExpression();
  // void ParseTupleExpression();
  // void ParseAwaitExpression();
  // void ParseYieldExpression();
  // void ParseCompareExpression();
  void ParseConstantExpression();
  // void ParseAttributeExpression();
  // void ParseStarredExpression();
  // void ParseNameExpression();
  // void ParseSliceExpression();

  // TODO(erik): Describe when finished.
  void PushStatement(StatementNode::Ptr statement);

  // TODO(erik): Describe when finished.
  ExpressionNode::Ptr PopExpression();
  void PushExpression(ExpressionNode::Ptr expression);

  // A stream of tokens generated from source code, which are converted
  // to statements and expressions in the syntax tree when read.
  StreamReader<Token> tokens_;

  // Top-level execution mode.
  Mode mode_;

  // The syntax tree. Incrementally built from `tokens_`.
  SyntaxTree syntax_tree_;

  // A block refers to a contiguous sequence of statements, indented by the same
  // amount.
  using Block = std::vector<StatementNode::Ptr>;
  std::stack<Block*> blocks_;

  // Previously parsed expressions that do not belong to a statement yet.
  std::stack<ExpressionNode::Ptr> exprs_;

  // TODO(erik): Change to array? Likewise for other maps keyed on token type.
  std::unordered_map<Token::Type, ParseStatementRule> stmt_rules_;
  std::unordered_map<Token::Type, ParseExpressionRule> expr_rules_;
};