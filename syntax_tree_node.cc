#include "syntax_tree_node.h"

#include "syntax_tree_visitor.h"

#define INSTANTIATE_VISIT(Klass) \
  void Klass::Visit(SyntaxTreeVisitor* visitor) { visitor->Visit(this); }

// Modules.
INSTANTIATE_VISIT(Module)
INSTANTIATE_VISIT(Interactive)
INSTANTIATE_VISIT(Expression)

// Statements.
INSTANTIATE_VISIT(Delete);
INSTANTIATE_VISIT(Assign)
INSTANTIATE_VISIT(Expr)

// Expressions.
INSTANTIATE_VISIT(BinaryOp)
INSTANTIATE_VISIT(UnaryOp)
INSTANTIATE_VISIT(Constant)
INSTANTIATE_VISIT(Name)

#undef INSTANTIATE_VISIT