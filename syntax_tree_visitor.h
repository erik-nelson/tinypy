#pragma once

#include "syntax_tree_node.h"

// Base (abstract) visitor class. A visitor defines a single Visit() callback
// for each possible type of syntax tree node.
struct SyntaxTreeVisitor {
  // Module nodes.
  virtual void Visit(Module* node) = 0;
  virtual void Visit(Interactive* node) = 0;
  virtual void Visit(Expression* node) = 0;

  // Statement nodes.
  virtual void Visit(Delete* node) = 0;
  virtual void Visit(Assign* node) = 0;
  virtual void Visit(If* node) = 0;
  virtual void Visit(Expr* node) = 0;

  // Expression nodes.
  virtual void Visit(BinaryOp* node) = 0;
  virtual void Visit(UnaryOp* node) = 0;
  virtual void Visit(Compare* node) = 0;
  virtual void Visit(Constant* node) = 0;
  virtual void Visit(Name* node) = 0;
};

#if 0
// Visitor that evaluates the syntax tree.
struct EvaluateVisitor : public SyntaxTreeVisitor {
};
#endif

// Visitor that builds a debug string for the syntax tree.
struct DebugStringVisitor : public SyntaxTreeVisitor {
  // Module nodes.
  void Visit(Module* node) override;
  void Visit(Interactive* node) override;
  void Visit(Expression* node) override;

  // Statement nodes.
  void Visit(Delete* node) override;
  void Visit(Assign* node) override;
  void Visit(If* node) override;
  void Visit(Expr* node) override;

  // Expression nodes.
  void Visit(BinaryOp* node) override;
  void Visit(UnaryOp* node) override;
  void Visit(Compare* node) override;
  void Visit(Constant* node) override;
  void Visit(Name* node) override;

  std::string str;
  size_t indentation = 0;
  void Append(std::string text);
  void AppendLine(std::string line);
};

// TODO(erik): TestSyntaxTreeVisitor?
// Visitor with stub implementations for all visit functions, used for testing.