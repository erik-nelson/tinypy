#pragma once

#include <functional>
#include <iostream>
#include <string>

#include "syntax_tree_node.h"
#include "syntax_tree_visitor.h"

class SyntaxTree {
 public:
  SyntaxTree();
  explicit SyntaxTree(SyntaxTreeNode::Ptr root);
  SyntaxTree(SyntaxTree&&) = default;
  SyntaxTree& operator=(SyntaxTree&&) noexcept = default;

  // Traverse the syntax tree, calling the provided visitor at each node.
  void Traverse(SyntaxTreeVisitor* visitor) const;

 private:
  // Parser can access our root node to build the tree.
  friend class Parser;
  SyntaxTreeNode::Ptr root_;
};

std::ostream& operator<<(std::ostream& os, const SyntaxTree& tree);