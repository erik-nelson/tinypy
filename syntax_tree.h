#pragma once

#include <functional>
#include <iostream>
#include <string>

#include "syntax_tree_node.h"

class SyntaxTree {
 public:
  SyntaxTree();

  // Debug printing.
  std::string DebugString() const;

 private:
  // Parser can access our root node to build the tree.
  friend class Parser;

  // Traverse the syntax tree, calling the provided visitor at each node.
  using VisitCallback = std::function<void(SyntaxTreeNode*)>;
  void Traverse(VisitCallback visit);

  SyntaxTreeNode::Ptr root_;
};

std::ostream& operator<<(std::ostream& os, const SyntaxTree& tree);