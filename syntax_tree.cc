#include "syntax_tree.h"

SyntaxTree::SyntaxTree() : root_(new Module) {}

std::string SyntaxTree::DebugString() const { return ""; }

void SyntaxTree::Traverse(VisitCallback callback) {}

std::ostream& operator<<(std::ostream& os, const SyntaxTree& tree) {
  os << tree.DebugString();
  return os;
}