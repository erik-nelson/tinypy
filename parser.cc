#include "parser.h"

#include <optional>

#include "syntax_tree_node.h"

namespace {
// Helper that pushes a provided value onto a provided deque.
template <typename T, typename U>
void Push(std::deque<T>* values, U&& value) {
  values->push_back(std::forward<U>(value));
}

// Helper that pops the top value from the provided deque, returning a default
// constructed value if none was available.
template <typename T>
T Pop(std::deque<T>* values) {
  if (values->empty()) return T{};
  T value = std::move(values->back());
  values->pop_back();
  return value;
}

// Specialization for unique_ptr values.
template <typename T, typename = std::enable_if_t<
                          !std::is_same_v<T, std::remove_reference_t<T>>>>
std::decay_t<T> Pop(std::deque<T>* values) {
  if (values->empty()) return std::decay_t<T>{};
  std::decay_t<T> value = std::move(values->back());
  values->pop_back();
  return value;
}
}  // namespace

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

  // ==, !=, <, <=, >, >=, is, is not, in, not in

  // Rule for DEL token.
  stmt_rules_[Token::Type::DEL] = [&] { ParseDeleteStatement(); };

  // Rule for IF token.
  stmt_rules_[Token::Type::IF] = [&] { ParseIfStatement(); };

  // Rule for IN token.
  expr_rules_[Token::Type::IN] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseCompareExpression(); },
                          .precedence = TokenPrecedence::COMPARISON};

  // Rule for IS token.
  expr_rules_[Token::Type::IS] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseCompareExpression(); },
                          .precedence = TokenPrecedence::COMPARISON};

  // Rule for IS_NOT token.
  expr_rules_[Token::Type::IS_NOT] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseCompareExpression(); },
                          .precedence = TokenPrecedence::COMPARISON};

  // Rule for NOT_IN token.
  expr_rules_[Token::Type::NOT_IN] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseCompareExpression(); },
                          .precedence = TokenPrecedence::COMPARISON};

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

  // Rule for LESS_THAN token.
  expr_rules_[Token::Type::LESS_THAN] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseCompareExpression(); },
                          .precedence = TokenPrecedence::COMPARISON};

  // Rule for LESS_EQUAL token.
  expr_rules_[Token::Type::LESS_EQUAL] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseCompareExpression(); },
                          .precedence = TokenPrecedence::COMPARISON};

  // Rule for GREATER_EQUAL token.
  expr_rules_[Token::Type::GREATER_EQUAL] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseCompareExpression(); },
                          .precedence = TokenPrecedence::COMPARISON};

  // Rule for GREATER_THAN token.
  expr_rules_[Token::Type::GREATER_THAN] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseCompareExpression(); },
                          .precedence = TokenPrecedence::COMPARISON};

  // Rule for EQUALS token.
  expr_rules_[Token::Type::EQUALS] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseCompareExpression(); },
                          .precedence = TokenPrecedence::COMPARISON};

  // Rule for NOT_EQUALS token.
  expr_rules_[Token::Type::NOT_EQUALS] =
      ParseExpressionRule{.prefix = nullptr,
                          .infix = [&] { ParseCompareExpression(); },
                          .precedence = TokenPrecedence::COMPARISON};

  // Rule for ASSIGN token.
  stmt_rules_[Token::Type::ASSIGN] = [&] { ParseAssignStatement(); };
}

void Parser::Parse() {
  // Top level node in the syntax tree corresponds to execution mode.
  if (mode_ == Mode::EXPRESSION) {
    // In EXPRESSION mode we expect a single expression.
    ParseExpression();

    auto root = std::make_unique<Expression>();
    root->body = Pop(&exprs_);
    syntax_tree_.root_ = std::move(root);
  }

  else {  // MODULE or INTERACTIVE mode.
    // In other modes we expect a block of statements.
    ParseBlock();

    if (mode_ == Mode::MODULE) {
      auto root = std::make_unique<Module>();
      root->body = Pop(&blocks_);
      syntax_tree_.root_ = std::move(root);
    } else {
      auto root = std::make_unique<Interactive>();
      root->body = Pop(&blocks_);
      syntax_tree_.root_ = std::move(root);
    }
  }
}

bool Parser::Peek(Token::Type type) const {
  return !tokens_.Depleted() && (*tokens_.Peek())->type == type;
}

bool Parser::Match(Token::Type type) const {
  if (Peek(type)) {
    tokens_.Advance();
    return true;
  }

  return false;
}

void Parser::Consume(Token::Type type) const {
  Expect(type);
  Match(type);
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

void Parser::ParseBlock() {
  // Parse statements until a dedent, or depleted.
  while (!tokens_.Depleted() && !Match(Token::Type::DEDENT)) {
    ParseStatement();
  }

  // Push all parsed statements onto the current block.
  Block block(std::make_move_iterator(stmts_.begin()),
              std::make_move_iterator(stmts_.end()));
  stmts_.clear();
  Push(&blocks_, std::move(block));
}

void Parser::ParseStatement() {
  while (!tokens_.Depleted() && !Match(Token::Type::NEWLINE)) {
    std::optional<const Token*> next_token = tokens_.Peek();
    if (!next_token) return;
    std::cout << **next_token;
    auto it = stmt_rules_.find((*next_token)->type);
    
    if (it != stmt_rules_.end()) {
      // Apply statement rule to the token.
      it->second();
      break;
    } 
    
    // Couldn't find a matching statement. Parse as an expression. Internally
    // this stores the expression so that subsequent statements can use it.
    ParseExpression();
  }

  // Parse any remaining expression into an expression statement.
  if (auto expr = Pop(&exprs_)) {
    auto stmt = std::make_unique<Expr>();
    stmt->expr = std::move(expr);
    Push(&stmts_, std::move(stmt));
  }
}

void Parser::ParseExpression(TokenPrecedence precedence) {
  std::optional<const Token*> next_token = tokens_.Peek();
  if (!next_token) return;

  // Syntax error if we can't find an expression match for this token.
  auto it = expr_rules_.find((*next_token)->type);
  if (it == expr_rules_.end()) {
    throw std::runtime_error("Encountered unexpected token: " +
                             (*next_token)->DebugString());
  }

  // Apply prefix rule.
  TokenPrecedence rule_precedence = TokenPrecedence::NONE;
  if (const ParseExpressionRule& rule = it->second; rule.prefix) {
    rule_precedence = rule.precedence;
    rule.prefix();
  } else {
    throw std::runtime_error("Expected unexpected token: " +
                             (*next_token)->DebugString());
  }

  // Apply infix rule(s).
  while (!tokens_.Depleted() &&
         static_cast<int>(rule_precedence) >= static_cast<int>(precedence)) {
    next_token = tokens_.Peek();
    const Token::Type next_type = (*next_token)->type;
    auto it = expr_rules_.find(next_type);
    if (it != expr_rules_.end()) {
      const ParseExpressionRule& rule = it->second;
      if (rule.infix == nullptr) {
        throw std::runtime_error("Encountered null infix for token: " +
                                 (*next_token)->DebugString());
      }
      rule_precedence = rule.precedence;
      rule.infix();
    } else {
      break;
    }
  }
}

void Parser::ParseDeleteStatement() {
  // Eat preceding DEL token.
  Consume(Token::Type::DEL);

  puts("Parse delete statement");

  // Parse comma-separated list of names.
  auto stmt = std::make_unique<Delete>();
  do {
    Expect(Token::Type::IDENTIFIER);
    ParseNameExpression();

    auto expr = Pop(&exprs_);
    dynamic_cast<Name*>(expr.get())->ctx_type = ExprContextType::DEL;
    stmt->targets.emplace_back(std::move(expr));
  } while (Match(Token::Type::COMMA));

  Push(&stmts_, std::move(stmt));
}

void Parser::ParseAssignStatement() {
  puts("Parse assign statement");

  // Match expressions until we run out of '=' tokens.
  // E.g. a = b = c = 3.
  std::vector<ExpressionNode::Ptr> exprs;
  exprs.emplace_back(Pop(&exprs_));
  while (Match(Token::Type::ASSIGN)) {
    ParseExpression();
    exprs.emplace_back(Pop(&exprs_));
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

  Push(&stmts_, std::move(stmt));
}

void Parser::ParseIfStatement() {
  puts("Parse if statement");

  // Eat preceding IF or ELIF token.
  tokens_.Advance();

  auto stmt = std::make_unique<If>();

  // Parse the if test.
  ParseExpression();
  stmt->test = Pop(&exprs_);

  Consume(Token::Type::COLON);
  if (!Match(Token::Type::NEWLINE)) {
    // The then branch appears on the same line:
    // 'if <cond>: <then>'
    //
    // In this case, an else branch is not allowed.
    ParseStatement();
    stmt->then_body.emplace_back(Pop(&stmts_));
  } 
  
  else {
    // The then branch appears on the next line:
    // if <cond>:
    //     <then>
    // else:
    //     <else>
    //
    // Parse the then branch body.
    Consume(Token::Type::INDENT);
    ParseBlock();
    stmt->then_body = Pop(&blocks_);

    // Parse the else branch body. The else branch can consist of either
    // an elif statement, in which case we recursively process a new if
    // statement as part of our else branch, or an else statement.
    if (Peek(Token::Type::ELIF)) {
      ParseIfStatement();
      stmt->else_body.emplace_back(Pop(&stmts_));
    } else if (Match(Token::Type::ELSE)) {
      // Parse else branch.
      Consume(Token::Type::COLON);
      Consume(Token::Type::NEWLINE);
      Consume(Token::Type::INDENT);

      ParseBlock();
      stmt->else_body = Pop(&blocks_);
    }
  }

  Push(&stmts_, std::move(stmt));
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

  expr->lhs = Pop(&exprs_);
  ParseExpression(expr_rules_[token->type].precedence);
  expr->rhs = Pop(&exprs_);
  Push(&exprs_, std::move(expr));
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
  expr->operand = Pop(&exprs_);
  Push(&exprs_, std::move(expr));
}

void Parser::ParseCompareExpression() {
  puts("Parse compare expression");

  auto expr = std::make_unique<Compare>();
  expr->lhs = Pop(&exprs_);

  // Keep matching comparison operators until we can't anymore. For example,
  // the expression 'a < b >= c not in d' has 3 comparison ops ('<', '>=',
  // 'not in'), and 3 comparators ('b', 'c', 'd').
  while (!tokens_.Depleted()) {
    std::optional<const Token*> op_token = tokens_.Peek();
    bool matched = true;
    switch ((*op_token)->type) {
      case Token::Type::EQUALS:
        expr->ops.emplace_back(CompareOpType::EQUALS);
        break;
      case Token::Type::NOT_EQUALS:
        expr->ops.emplace_back(CompareOpType::NOT_EQUALS);
        break;
      case Token::Type::LESS_THAN:
        expr->ops.emplace_back(CompareOpType::LESS_THAN);
        break;
      case Token::Type::LESS_EQUAL:
        expr->ops.emplace_back(CompareOpType::LESS_EQUAL);
        break;
      case Token::Type::GREATER_THAN:
        expr->ops.emplace_back(CompareOpType::GREATER_THAN);
        break;
      case Token::Type::GREATER_EQUAL:
        expr->ops.emplace_back(CompareOpType::GREATER_EQUAL);
        break;
      case Token::Type::IS:
        expr->ops.emplace_back(CompareOpType::IS);
        break;
      case Token::Type::IS_NOT:
        expr->ops.emplace_back(CompareOpType::IS_NOT);
        break;
      case Token::Type::IN:
        expr->ops.emplace_back(CompareOpType::IN);
        break;
      case Token::Type::NOT_IN:
        expr->ops.emplace_back(CompareOpType::NOT_IN);
        break;
      default:
        matched = false;
        break;
    }

    if (!matched) break;
    tokens_.Advance();

    // Parse the comparator expression (after the comparison operator).
    ParseExpression(TokenPrecedence::COMPARISON);
    expr->comparators.emplace_back(Pop(&exprs_));
  }

  // Make sure that we had at least one comparator on the right hand side.
  if (expr->ops.empty() || expr->comparators.empty()) {
    throw std::runtime_error(
        "Encountered comparison token, but found no comparator.");
  }

  Push(&exprs_, std::move(expr));
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

  Push(&exprs_, std::move(expr));
}

void Parser::ParseNameExpression() {
  std::optional<Token> token = tokens_.Read();

  puts("Parse name expression for token:");
  std::cout << "\t" << *token;

  auto expr = std::make_unique<Name>();
  expr->id = std::move(token->value.value());
  expr->ctx_type = ExprContextType::LOAD;
  Push(&exprs_, std::move(expr));
}