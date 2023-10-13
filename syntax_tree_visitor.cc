#include "syntax_tree_visitor.h"

namespace {
// Hard coded num spaces for print indentation.
constexpr size_t kIndentationWidth = 4;

// Helper conversion from `ConstantValue` type to string.
std::string ConstantValueString(const ConstantValue& constant) {
  struct DebugVisitor {
    std::string operator()(std::string value) {
      return "String: " + std::move(value);
    }
    std::string operator()(double value) {
      return "Double: " + std::to_string(value);
    }
    std::string operator()(int value) {
      return "Int: " + std::to_string(value);
    }
    std::string operator()(bool value) {
      return std::string("Bool: ") + (value ? "true" : "false");
    }
    std::string operator()(const NoneType&) { return "None"; }
  };
  return std::visit(DebugVisitor{}, constant);
}
}  // namespace

void DebugStringVisitor::Visit(Module* node) {
  Append("Module(");
  indentation += 1;
  AppendLine("body=[");
  indentation += 1;
  for (const auto& stmt : node->body) {
    AppendLine("");
    stmt->Visit(this);
  }
  Append("])\n");
  indentation -= 2;
}

void DebugStringVisitor::Visit(Interactive* node) {
  Append("Interactive(");
  indentation += 1;
  AppendLine("body=[");
  indentation += 1;
  for (const auto& stmt : node->body) {
    AppendLine("");
    stmt->Visit(this);
  }
  Append("])\n");
  indentation -= 2;
}

void DebugStringVisitor::Visit(Expression* node) {
  Append("Expression(");
  indentation += 1;
  AppendLine("body=");
  indentation += 1;
  if (node->body) node->body->Visit(this);
  Append(")\n");
  indentation -= 2;
}

void DebugStringVisitor::Visit(Delete* node) {
  Append("Delete(");
  indentation += 1;
  AppendLine("targets=[");
  indentation += 1;
  for (const auto& expr : node->targets) {
    AppendLine("");
    expr->Visit(this);
  }
  Append("])");
  indentation -= 2;
}

void DebugStringVisitor::Visit(Assign* node) {
  Append("Assign(");
  indentation += 1;
  AppendLine("targets=[");
  indentation += 1;
  for (const auto& expr : node->targets) {
    AppendLine("");
    expr->Visit(this);
  }
  Append("],");
  indentation -= 1;
  AppendLine("value=");
  node->value->Visit(this);
  Append(")");
  indentation -= 1;
}

void DebugStringVisitor::Visit(Expr* node) {
  Append("Expr(");
  indentation += 1;
  AppendLine("value=");
  node->expr->Visit(this);
  Append(")");
  indentation -= 1;
}

void DebugStringVisitor::Visit(BinaryOp* node) {
  Append("BinaryOp(");
  indentation += 1;
  AppendLine("lhs=");
  node->lhs->Visit(this);
  Append(",");
  AppendLine("op=");
  Append([&] {
    switch (node->op_type) {
      case BinaryOpType::ADD:
        return "Add,";
      case BinaryOpType::SUBTRACT:
        return "Subtract,";
      case BinaryOpType::MULTIPLY:
        return "Multiply,";
      case BinaryOpType::MATMUL:
        return "Matmul,";
      case BinaryOpType::DIVIDE:
        return "Divide,";
      case BinaryOpType::MODULO:
        return "Modulo,";
      case BinaryOpType::POWER:
        return "Power,";
      case BinaryOpType::LEFT_SHIFT:
        return "Left shift,";
      case BinaryOpType::RIGHT_SHIFT:
        return "Right shift,";
      case BinaryOpType::BITWISE_OR:
        return "Bitwise or,";
      case BinaryOpType::BITWISE_XOR:
        return "Bitwise xor,";
      case BinaryOpType::BITWISE_AND:
        return "Bitwise and,";
      case BinaryOpType::FLOOR_DIVIDE:
        return "Floor divide,";
    }
    return "";
  }());
  AppendLine("rhs=");
  node->rhs->Visit(this);
  indentation -= 1;
  Append(")");
}

void DebugStringVisitor::Visit(UnaryOp* node) {
  Append("UnaryOp(");
  indentation += 1;
  AppendLine("op=");
  Append([&] {
    switch (node->op_type) {
      case UnaryOpType::INVERT:
        return "Invert,";
      case UnaryOpType::NOT:
        return "Not,";
      case UnaryOpType::POSITIVE:
        return "Positive,";
      case UnaryOpType::NEGATIVE:
        return "Negative,";
    }
    return "";
  }());
  AppendLine("operand=");
  node->operand->Visit(this);
  indentation -= 1;
  Append(")");
}

void DebugStringVisitor::Visit(Constant* node) {
  Append("Constant(value=");
  Append(ConstantValueString(node->value));
  Append(")");
}

void DebugStringVisitor::Visit(Name* node) {
  puts("name???");
  Append("Name(id='");
  Append(node->id);
  Append("', ctx=");
  Append([&] {
    switch (node->ctx_type) {
      case ExprContextType::LOAD:
        return "Load";
      case ExprContextType::STORE:
        return "Store";
      case ExprContextType::DEL:
        return "Del";
    }
    return "";
  }());
  Append(")");
}

void DebugStringVisitor::Append(std::string text) { str += std::move(text); }

void DebugStringVisitor::AppendLine(std::string line) {
  Append("\n");
  Append(std::string(indentation * kIndentationWidth, ' '));
  Append(std::move(line));
}