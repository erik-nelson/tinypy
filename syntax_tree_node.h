#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "types.h"

// Inheritance structure of syntax tree nodes gathered from:
// https://docs.python.org/3/library/ast.html

// ----------------------------------------------------------------------------
// Tags for various syntax tree node subcontexts.
// ----------------------------------------------------------------------------
enum class ExprContextType { LOAD, STORE, DEL };

enum class BooleanOpType { AND, OR };

enum class UnaryOpType {
  INVERT,
  NOT,
  ADD,
  NEGATE,
};

enum class BinaryOpType {
  ADD,
  SUBTRACT,
  MULTIPLY,
  MATMUL,
  DIVIDE,
  MODULO,
  POWER,
  LEFT_SHIFT,
  RIGHT_SHIFT,
  BITWISE_OR,
  BITWISE_XOR,
  BITWISE_AND,
  FLOOR_DIVIDE
};

enum class CompareOpType {
  EQUALS,
  NOT_EQUALS,
  LESS_THAN,
  LESS_EQUAL,
  GREATER_THAN,
  GREATER_EQUAL,
  IS,
  IS_NOT,
  IN,
  NOT_IN,
};

// TODO(erik):
// - Comprehension
// - Exception handlers
// - Arguments
// - Arg
// - Keywords
// - Aliases
// - With item
// - Match case
// - Pattern
// - Type ignore

// ----------------------------------------------------------------------------
// Base syntax tree node.
// ----------------------------------------------------------------------------
struct SyntaxTreeNode {
  using Ptr = std::unique_ptr<SyntaxTreeNode>;
};

// ----------------------------------------------------------------------------
// Intermediate nodes (not concrete).
// ----------------------------------------------------------------------------
struct ModuleNode : public SyntaxTreeNode {
  using Ptr = std::unique_ptr<ModuleNode>;
};
struct StatementNode : public SyntaxTreeNode {
  using Ptr = std::unique_ptr<StatementNode>;
};
struct ExpressionNode : public SyntaxTreeNode {
  using Ptr = std::unique_ptr<ExpressionNode>;
};

// ----------------------------------------------------------------------------
// Module nodes.
// ----------------------------------------------------------------------------
struct Module : public ModuleNode {
  std::vector<StatementNode::Ptr> body;
};

struct Interactive : public ModuleNode {
  std::vector<StatementNode::Ptr> body;
};

struct Expression : public ModuleNode {
  ExpressionNode::Ptr body;
};

// TODO(erik): Do we need this? This is for type annotations.
// struct FunctionType : public ModuleNode {};

// ----------------------------------------------------------------------------
// Statement nodes.
// ----------------------------------------------------------------------------
// struct FunctionDef : public StatementNode {};
// struct AsyncFunctionDef : public StatementNode {};
// struct ClassDef : public StatementNode {};
// struct Return : public StatementNode {};
// struct Delete : public StatementNode {};
struct Assign : public StatementNode {
  std::vector<ExpressionNode::Ptr> targets;
  ExpressionNode::Ptr value;
};
// struct AugAssign : public StatementNode {};
// struct AnnAssign : public StatementNode {};
// struct For : public StatementNode {};
// struct AsyncFor : public StatementNode {};
// struct While : public StatementNode {};
// struct If : public StatementNode {};
// struct With : public StatementNode {};
// struct AsyncWith : public StatementNode {};
// struct Match : public StatementNode {};
// struct Raise : public StatementNode {};
// struct Try : public StatementNode {};
// struct TryStar : public StatementNode {};
// struct Assert : public StatementNode {};
// struct Import : public StatementNode {};
// struct ImportFrom : public StatementNode {};
// struct Global : public StatementNode {};
// struct Nonlocal : public StatementNode {};
struct Expr : public StatementNode {
  ExpressionNode::Ptr expr;
};
// struct Pass : public StatementNode {};
// struct Break : public StatementNode {};
// struct Continue : public StatementNode {};

// ----------------------------------------------------------------------------
// Expression nodes.
// ----------------------------------------------------------------------------
// struct BooleanOp : public ExpressionNode {};
// struct NamedExpr : public ExpressionNode {};
struct BinaryOp : public ExpressionNode {
  ExpressionNode::Ptr lhs, rhs;
  BinaryOpType op_type;
};
struct UnaryOp : public ExpressionNode {
  ExpressionNode::Ptr operand;
  UnaryOpType op_type;
};
// struct Lambda : public ExpressionNode {};
// struct IfExp : public ExpressionNode {};
// struct List : public ExpressionNode {};
// struct Dict : public ExpressionNode {};
// struct Set : public ExpressionNode {};
// struct Tuple : public ExpressionNode {};
// struct ListComp : public ExpressionNode {};
// struct SetComp : public ExpressionNode {};
// struct DictComp : public ExpressionNode {};
// struct Generator : public ExpressionNode {};
// struct Await : public ExpressionNode {};
// struct Yield : public ExpressionNode {};
// struct YieldFrom : public ExpressionNode {};
// struct Compare : public ExpressionNode {};
// struct Call : public ExpressionNode {};
// struct FormattedValue : public ExpressionNode {};
// struct JoinedStr : public ExpressionNode {};
struct Constant : public ExpressionNode {
  ConstantValue value;
};
// struct Attribute : public ExpressionNode {};
// struct Subscript : public ExpressionNode {};
// struct Starred : public ExpressionNode {};
struct Name : public ExpressionNode {
  Identifier id;
  ExprContextType ctx_type;
};
// struct Slice : public ExpressionNode {};

// ----------------------------------------------------------------------------
// Debug printing.
// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const SyntaxTreeNode& node);