#include "parser.h"

#include <optional>

#include "syntax_tree_node.h"

Parser::Parser(StreamReader<Token> tokens, Mode mode)
    : tokens_(std::move(tokens)), mode_(mode) {
#if 0  // TODO(erik): Reorganize.
  // Statement rules.
  stmt_rules_[Token::Type::DEF];  // function def
  stmt_rules_[Token::Type::ASYNC];  // async function def, async for, async with
  stmt_rules_[Token::Type::CLASS];  // class def
  stmt_rules_[Token::Type::RETURN];  // return
  stmt_rules_[Token::Type::DEL];  // delete
  stmt_rules_[Token::Type::ASSIGN];  // assign
  stmt_rules_[Token::Type::PLUS_ASSIGN];  // aug assign
  stmt_rules_[Token::Type::MINUS_ASSIGN];  // aug assign
  stmt_rules_[Token::Type::MULTIPLY_ASSIGN];  // aug assign
  stmt_rules_[Token::Type::DIVIDE_ASSIGN];  // aug assign
  stmt_rules_[Token::Type::FLOOR_DIVIDE_ASSIGN];  // aug assign
  stmt_rules_[Token::Type::MODULO_ASSIGN];  // aug assign
  stmt_rules_[Token::Type::MATMUL_ASSIGN];  // aug assign
  stmt_rules_[Token::Type::AND_ASSIGN];  // aug assign
  stmt_rules_[Token::Type::OR_ASSIGN];  // aug assign
  stmt_rules_[Token::Type::XOR_ASSIGN];  // aug assign
  stmt_rules_[Token::Type::RIGHT_SHIFT_ASSIGN];  // aug assign
  stmt_rules_[Token::Type::LEFT_SHIFT_ASSIGN];  // aug assign
  stmt_rules_[Token::Type::POWER_ASSIGN];  // aug assign
  stmt_rules_[Token::Type::FOR];  // for
  stmt_rules_[Token::Type::WHILE];  // while
  stmt_rules_[Token::Type::IF];  // if
  stmt_rules_[Token::Type::WITH];  // with
  stmt_rules_[Token::Type::RAISE];  // raise
  stmt_rules_[Token::Type::TRY];  // try, trystar
  stmt_rules_[Token::Type::ASSERT];  // assert
  stmt_rules_[Token::Type::IMPORT];  // import, import from
  stmt_rules_[Token::Type::GLOBAL];  // global
  stmt_rules_[Token::Type::NONLOCAL];  // nonlocal
  stmt_rules_[Token::Type::PASS];  // pass
  stmt_rules_[Token::Type::BREAK];  // break
  stmt_rules_[Token::Type::CONTINUE];  // continue

  // Expression rules.
  expr_rules_[Token::Type::AND];  // boolean op
  expr_rules_[Token::Type::OR];  // boolean op
  expr_rules_[Token::Type::NAMED_EXPR];  // named expr
  expr_rules_[Token::Type::PLUS];  // binary op, unary op
  expr_rules_[Token::Type::MINUS];  // binary op, unary op
  expr_rules_[Token::Type::MULTIPLY];  // binary op, starred
  expr_rules_[Token::Type::MATMUL]; // binary op
  expr_rules_[Token::Type::DIVIDE];  // binary op
  expr_rules_[Token::Type::MODULO];  // binary op
  expr_rules_[Token::Type::POWER];  // binary op, starred
  expr_rules_[Token::Type::LEFT_SHIFT]; // binary op
  expr_rules_[Token::Type::RIGHT_SHIFT];  // binary op
  expr_rules_[Token::Type::BITWISE_OR];  // binary op
  expr_rules_[Token::Type::BITWISE_XOR];  // binary op
  expr_rules_[Token::Type::BITWISE_AND];  // binary op
  expr_rules_[Token::Type::FLOOR_DIVIDE];  // binary op
  expr_rules_[Token::Type::INVERT];  // unary op
  expr_rules_[Token::Type::NOT];  // unary op
  expr_rules_[Token::Type::LAMBDA];  // lambda
  expr_rules_[Token::Type::IF];  // ifexp (also a member of `stmt_rules_`).
  expr_rules_[Token::Type::LEFT_BRACKET];  // list, listcomp, subscript
  expr_rules_[Token::Type::LEFT_BRACE];  // dict, set, dictcomp, setcomp
  expr_rules_[Token::Type::LEFT_PAREN];  // tuple, generatorexp, call
  expr_rules_[Token::Type::AWAIT];  // await
  expr_rules_[Token::Type::YIELD];  // yield, yield from
  expr_rules_[Token::Type::EQUALS];  // compare
  expr_rules_[Token::Type::NOT_EQUALS];  // compare
  expr_rules_[Token::Type::LESS_THAN];  // compare
  expr_rules_[Token::Type::LESS_EQUAL];  // compare
  expr_rules_[Token::Type::GREATER_THAN];  // compare
  expr_rules_[Token::Type::GREATER_EQUAL];  // compare
  expr_rules_[Token::Type::IS];  // compare
  expr_rules_[Token::Type::IS_NOT];  // compare
  expr_rules_[Token::Type::NOT_IN];  // compare
  expr_rules_[Token::Type::INTEGER];  // constant
  expr_rules_[Token::Type::FLOAT];  //constant
  expr_rules_[Token::Type::FALSE];  // constant
  expr_rules_[Token::Type::TRUE];  // constant
  expr_rules_[Token::Type::STRING]; // constant - TODO: formattedvalue / joinedstr?
  expr_rules_[Token::Type::ATTRIBUTE];  // attribute
  expr_rules_[Token::Type::IDENTIFIER];  // name
  expr_rules_[Token::Type::COLON];  // slice
#endif

  // Rule for DEL token.
  stmt_rules_[Token::Type::DEL] = [&] { ParseDeleteStatement(); };

  // Rule for ASSIGN token.
  stmt_rules_[Token::Type::ASSIGN] = [&] { ParseAssignStatement(); };

  // Rule for NOT token.
  expr_rules_[Token::Type::NOT] =
      ParseExpressionRule{.prefix = [&] { ParseUnaryOpExpression(); },
                          .infix = nullptr,
                          .precedence = TokenPrecedence::NOT};

  // Rule for IDENTIFIER token.
  expr_rules_[Token::Type::IDENTIFIER] =
      ParseExpressionRule{.prefix = [&] { ParseNameExpression(); },
                          .infix = nullptr,
                          .precedence = TokenPrecedence::NONE};

  // Rule for INTEGER token.
  expr_rules_[Token::Type::INTEGER] =
      ParseExpressionRule{.prefix = [&] { ParseConstantExpression(); },
                          .infix = nullptr,
                          .precedence = TokenPrecedence::NONE};

  // Rule for FLOAT token.
  expr_rules_[Token::Type::FLOAT] =
      ParseExpressionRule{.prefix = [&] { ParseConstantExpression(); },
                          .infix = nullptr,
                          .precedence = TokenPrecedence::NONE};

  // Rule for STRING token.
  expr_rules_[Token::Type::STRING] =
      ParseExpressionRule{.prefix = [&] { ParseConstantExpression(); },
                          .infix = nullptr,
                          .precedence = TokenPrecedence::NONE};

  // Rule for PLUS token.
  expr_rules_[Token::Type::PLUS] =
      ParseExpressionRule{.prefix = [&] { ParseUnaryOpExpression(); },
                          .infix = [&] { ParseBinaryOpExpression(); },
                          .precedence = TokenPrecedence::ADD_SUBTRACT};

  // Rule for MINUS token.
  expr_rules_[Token::Type::MINUS] =
      ParseExpressionRule{.prefix = [&] { ParseUnaryOpExpression(); },
                          .infix = [&] { ParseBinaryOpExpression(); },
                          .precedence = TokenPrecedence::ADD_SUBTRACT};

  // Rule for MULTIPLY token.
  expr_rules_[Token::Type::MULTIPLY] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseBinaryOpExpression(); },
                          .precedence = TokenPrecedence::MULTIPLY_DIVIDE};

  // Rule for POWER token.
  expr_rules_[Token::Type::POWER] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseBinaryOpExpression(); },
                          .precedence = TokenPrecedence::POWER};

  // Rule for DIVIDE token.
  expr_rules_[Token::Type::DIVIDE] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseBinaryOpExpression(); },
                          .precedence = TokenPrecedence::MULTIPLY_DIVIDE};

  // Rule for FLOOR_DIVIDE token.
  expr_rules_[Token::Type::FLOOR_DIVIDE] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseBinaryOpExpression(); },
                          .precedence = TokenPrecedence::MULTIPLY_DIVIDE};

  // Rule for MODULO token.
  expr_rules_[Token::Type::MODULO] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseBinaryOpExpression(); },
                          .precedence = TokenPrecedence::MULTIPLY_DIVIDE};

  // Rule for MATMUL token.
  expr_rules_[Token::Type::MATMUL] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseBinaryOpExpression(); },
                          .precedence = TokenPrecedence::MULTIPLY_DIVIDE};

  // Rule for LEFT_SHIFT token.
  expr_rules_[Token::Type::LEFT_SHIFT] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseBinaryOpExpression(); },
                          .precedence = TokenPrecedence::BITWISE_SHIFT};

  // Rule for RIGHT_SHIFT token.
  expr_rules_[Token::Type::RIGHT_SHIFT] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseBinaryOpExpression(); },
                          .precedence = TokenPrecedence::BITWISE_SHIFT};

  // Rule for BITWISE_AND token.
  expr_rules_[Token::Type::BITWISE_AND] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseBinaryOpExpression(); },
                          .precedence = TokenPrecedence::BITWISE_AND};

  // Rule for BITWISE_OR token.
  expr_rules_[Token::Type::BITWISE_OR] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseBinaryOpExpression(); },
                          .precedence = TokenPrecedence::BITWISE_OR};

  // Rule for BITWISE_XOR token.
  expr_rules_[Token::Type::BITWISE_XOR] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseBinaryOpExpression(); },
                          .precedence = TokenPrecedence::BITWISE_XOR};

  // Rule for INVERT token.
  expr_rules_[Token::Type::INVERT] =
      ParseExpressionRule{.prefix = [&] { ParseUnaryOpExpression(); },
                          .infix = nullptr,
                          .precedence = TokenPrecedence::BITWISE_NOT};
}

SyntaxTree& Parser::Parse() & {
  // Top level node in the syntax tree corresponds to execution mode.
  if (mode_ == Mode::EXPRESSION) {
    // In EXPRESSION mode we expect a single expression.
    auto root = std::make_unique<Expression>();
    ParseExpression();
    root->body = PopExpression();
    syntax_tree_.root_ = std::move(root);
  }

  else {  // MODULE or INTERACTIVE mode.
    // In other modes we expect a sequence of statements.
    if (mode_ == Mode::MODULE) {
      auto root = std::make_unique<Module>();
      blocks_.push(&root->body);
      syntax_tree_.root_ = std::move(root);
    } else {
      auto root = std::make_unique<Interactive>();
      blocks_.push(&root->body);
      syntax_tree_.root_ = std::move(root);
    }

    // Parse all available statements.
    while (!tokens_.Depleted()) ParseStatement();

    // If at the end of parsing we still have leftover expressions,
    // convert them to a sequence of individual expression statements.
    while (auto expr = PopExpression()) {
      auto stmt = std::make_unique<Expr>();
      stmt->expr = std::move(expr);
      PushStatement(std::move(stmt));
    }
  }

  return syntax_tree_;
}

SyntaxTree&& Parser::Parse() && {
  Parse();
  return std::move(syntax_tree_);
}

bool Parser::Match(Token::Type type) const {
  if (auto next_token = tokens_.Peek();
      next_token.has_value() && next_token.value()->type == type) {
    tokens_.Advance();
    return true;
  }
  return false;
}

void Parser::Expect(Token::Type type) const {
  std::optional<const Token*> next_token = tokens_.Peek();
  if (!next_token.has_value()) {
    std::string err = "Failed to match token ";
    err += kTokenTypeToString.at(type);
    err += " (no more tokens available).";
    throw std::runtime_error(err);
  }
  if (next_token.value()->type != type) {
    std::string err = "Failed to match token ";
    err += kTokenTypeToString.at(type);
    err += " (got " + kTokenTypeToString.at(next_token.value()->type) + ").";
    throw std::runtime_error(err);
  }
}

void Parser::ParseStatement() {
  std::optional<const Token*> next_token = tokens_.Peek();
  if (!next_token) return;

  auto it = stmt_rules_.find((*next_token)->type);
  if (it != stmt_rules_.end()) {
    // Apply statement rule to the token.
    it->second();
  } else {
    // Couldn't find a matching statement. Parse as an expression. Internally
    // this stores the expression so that subsequent statements can use it.
    ParseExpression();
  }
}

void Parser::ParseExpression(TokenPrecedence precedence) {
  std::optional<const Token*> next_token = tokens_.Peek();
  if (!next_token) return;

  // Syntax error if we can't find an expression match for this token.
  auto it = expr_rules_.find((*next_token)->type);
  if (it == expr_rules_.end()) {
    throw std::runtime_error("Failed to parse token: " +
                             (*next_token)->DebugString());
  }

  // Apply prefix rule.
  TokenPrecedence rule_precedence = TokenPrecedence::NONE;
  if (const ParseExpressionRule& rule = it->second; rule.prefix) {
    rule_precedence = rule.precedence;
    rule.prefix();
  } else {
    throw std::runtime_error("Expected expression, got: " +
                             (*next_token)->DebugString());
  }

  // Apply infix rule(s).
  while (!tokens_.Depleted() &&
         static_cast<int>(rule_precedence) >= static_cast<int>(precedence)) {
    const Token::Type next_type = (*tokens_.Peek())->type;
    auto it = expr_rules_.find(next_type);
    if (it != expr_rules_.end()) {
      const ParseExpressionRule& rule = it->second;
      rule_precedence = rule.precedence;
      rule.infix();
    } else {
      break;
    }
  }
}

void Parser::ParseDeleteStatement() {
  // Eat preceding DEL token.
  Match(Token::Type::DEL);

  puts("Parse delete statement");

  // Parse comma-separated list of names.
  auto stmt = std::make_unique<Delete>();
  do {
    Expect(Token::Type::IDENTIFIER);
    ParseNameExpression();

    auto expr = PopExpression();
    dynamic_cast<Name*>(expr.get())->ctx_type = ExprContextType::DEL;
    stmt->targets.emplace_back(std::move(expr));
  } while (Match(Token::Type::COMMA));

  PushStatement(std::move(stmt));
}

void Parser::ParseAssignStatement() {
  puts("Parse assign statement");

  // Match expressions until we run out of '=' tokens.
  // E.g. a = b = c = 3.
  std::vector<ExpressionNode::Ptr> exprs;
  exprs.emplace_back(PopExpression());
  while (Match(Token::Type::ASSIGN)) {
    ParseExpression();
    exprs.emplace_back(PopExpression());
  }

  // The final parsed expression is the value of the assignment.
  auto stmt = std::make_unique<Assign>();
  stmt->value = std::move(exprs.back());
  exprs.pop_back();

  // All preceding expressions are the targets.
  stmt->targets = std::move(exprs);

  // Any variables we are storing to need a STORE context.
  for (auto& expr : stmt->targets) {
    if (auto* name = dynamic_cast<Name*>(expr.get())) {
      name->ctx_type = ExprContextType::STORE;
    }
  }

  PushStatement(std::move(stmt));
}

void Parser::ParseBinaryOpExpression() {
  std::optional<Token> token = tokens_.Read();

  puts("Parse binary expression for token:");
  std::cout << "\t" << *token;

  auto expr = std::make_unique<BinaryOp>();
  expr->op_type = [&]() {
    switch (token->type) {
      case Token::Type::PLUS:
        return BinaryOpType::ADD;
      case Token::Type::MINUS:
        return BinaryOpType::SUBTRACT;
      case Token::Type::MULTIPLY:
        return BinaryOpType::MULTIPLY;
      case Token::Type::MATMUL:
        return BinaryOpType::MATMUL;
      case Token::Type::DIVIDE:
        return BinaryOpType::DIVIDE;
      case Token::Type::MODULO:
        return BinaryOpType::MODULO;
      case Token::Type::POWER:
        return BinaryOpType::POWER;
      case Token::Type::LEFT_SHIFT:
        return BinaryOpType::LEFT_SHIFT;
      case Token::Type::RIGHT_SHIFT:
        return BinaryOpType::RIGHT_SHIFT;
      case Token::Type::BITWISE_OR:
        return BinaryOpType::BITWISE_OR;
      case Token::Type::BITWISE_XOR:
        return BinaryOpType::BITWISE_XOR;
      case Token::Type::BITWISE_AND:
        return BinaryOpType::BITWISE_AND;
      case Token::Type::FLOOR_DIVIDE:
        return BinaryOpType::FLOOR_DIVIDE;
      default:
        throw std::runtime_error("Encountered unexpected binary operation: " +
                                 token->DebugString());
    }
  }();

  expr->lhs = PopExpression();
  ParseExpression(expr_rules_[token->type].precedence);
  expr->rhs = PopExpression();
  PushExpression(std::move(expr));
}

void Parser::ParseUnaryOpExpression() {
  std::optional<Token> token = tokens_.Read();

  puts("Parse unary expression for token:");
  std::cout << "\t" << *token;

  auto expr = std::make_unique<UnaryOp>();
  expr->op_type = [&]() {
    switch (token->type) {
      case Token::Type::PLUS:
        return UnaryOpType::POSITIVE;
      case Token::Type::MINUS:
        return UnaryOpType::NEGATIVE;
      case Token::Type::NOT:
        return UnaryOpType::NOT;
      case Token::Type::INVERT:
        return UnaryOpType::INVERT;
      default:
        throw std::runtime_error("Encountered unexpected unary operation: " +
                                 token->DebugString());
    }
  }();

  ParseExpression(expr_rules_[token->type].precedence);
  expr->operand = PopExpression();
  PushExpression(std::move(expr));
}

void Parser::ParseConstantExpression() {
  std::optional<Token> token = tokens_.Read();

  puts("Parse constant expression for token:");
  std::cout << "\t" << *token;

  auto expr = std::make_unique<Constant>();
  expr->value = [&]() -> ConstantValue {
    switch (token->type) {
      case Token::Type::INTEGER: {
        return std::stoi(token->value.value());
      }
      case Token::Type::FLOAT: {
        return std::stod(token->value.value());
      }
      case Token::Type::STRING: {
        return token->value.value();
      }
      default:
        return NoneType();
    }
  }();
  PushExpression(std::move(expr));
}

void Parser::ParseNameExpression() {
  std::optional<Token> token = tokens_.Read();

  puts("Parse name expression for token:");
  std::cout << "\t" << *token;

  auto expr = std::make_unique<Name>();
  expr->id = std::move(token->value.value());
  expr->ctx_type = ExprContextType::LOAD;
  PushExpression(std::move(expr));
}

void Parser::PushStatement(StatementNode::Ptr statement) {
  blocks_.top()->emplace_back(std::move(statement));
}

ExpressionNode::Ptr Parser::PopExpression() {
  if (exprs_.empty()) return nullptr;
  ExpressionNode::Ptr expression = std::move(exprs_.top());
  exprs_.pop();
  return expression;
}

void Parser::PushExpression(ExpressionNode::Ptr expression) {
  exprs_.push(std::move(expression));
}