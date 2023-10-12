#include "syntax_tree.h"

SyntaxTree::SyntaxTree() : root_(new Module) {}

SyntaxTree::SyntaxTree(SyntaxTreeNode::Ptr root) : root_(std::move(root)) {}

void SyntaxTree::Traverse(SyntaxTreeVisitor* visitor) const {
  root_->Visit(visitor);
}

std::ostream& operator<<(std::ostream& os, const SyntaxTree& tree) {
  DebugStringVisitor visitor;
  tree.Traverse(&visitor);
  os << visitor.str;
  return os;
}