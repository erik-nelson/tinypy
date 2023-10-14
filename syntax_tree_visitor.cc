#include "syntax_tree_visitor.h"

#include <functional>

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

// Function that prints a single element of type T.
template <typename T>
using PrintElement = std::function<void(const T&, DebugStringVisitor*)>;

// Default print function for a single element.
template <typename T>
PrintElement<T> DefaultPrint() {
  return [](const T& element, DebugStringVisitor* visitor) {
    element->Visit(visitor);
  };
}

// Append a list element to the debug string visitor.
template <typename T>
void AppendList(const std::string& name, const std::vector<T>& list,
                DebugStringVisitor* visitor,
                PrintElement<T> print_element = DefaultPrint<T>()) {
  visitor->AppendLine(name);
  visitor->Append("=[");
  visitor->indentation += 1;
  if (!list.empty()) {
    for (size_t i = 0; i < list.size() - 1; ++i) {
      visitor->AppendLine("");
      print_element(list[i], visitor);
      visitor->Append(",");
    }
    visitor->AppendLine("");
    print_element(list.back(), visitor);
  }
  visitor->Append("]");
  visitor->indentation -= 1;
}
}  // namespace

void DebugStringVisitor::Visit(Module* node) {
  Append("Module(");
  indentation += 1;

  AppendList("body", node->body, this);

  Append(")");
  indentation -= 1;
  AppendLine("");
}

void DebugStringVisitor::Visit(Interactive* node) {
  Append("Interactive(");
  indentation += 1;

  AppendList("body", node->body, this);

  Append(")");
  indentation -= 1;
  AppendLine("");
}

void DebugStringVisitor::Visit(Expression* node) {
  Append("Expression(");
  indentation += 1;

  AppendLine("body=");
  indentation += 1;
  if (node->body) node->body->Visit(this);

  Append(")");
  indentation -= 2;
  AppendLine("");
}

void DebugStringVisitor::Visit(Delete* node) {
  Append("Delete(");
  indentation += 1;

  AppendList("targets", node->targets, this);

  Append(")");
  indentation -= 1;
}

void DebugStringVisitor::Visit(Assign* node) {
  Append("Assign(");
  indentation += 1;

  AppendList("targets", node->targets, this);
  Append(",");

  AppendLine("value=");
  node->value->Visit(this);
  Append(")");
  indentation -= 1;
}

void DebugStringVisitor::Visit(If* node) {
  Append("If(");
  indentation += 1;

  AppendLine("test=");
  node->test->Visit(this);
  Append(",");

  AppendList("then", node->then_body, this);
  Append(",");
  
  AppendList("else", node->else_body, this);

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

void DebugStringVisitor::Visit(Compare* node) {
  Append("Compare(");
  indentation += 1;

  AppendLine("lhs="), node->lhs->Visit(this);
  Append(",");

  PrintElement<CompareOpType> print_op = [&](const CompareOpType& op_type,
                                             DebugStringVisitor* visitor) {
    visitor->Append([&] {
      switch (op_type) {
        case CompareOpType::EQUALS:
          return "Equals";
        case CompareOpType::NOT_EQUALS:
          return "Not equals";
        case CompareOpType::LESS_THAN:
          return "Less than";
        case CompareOpType::LESS_EQUAL:
          return "Less equal";
        case CompareOpType::GREATER_THAN:
          return "Greater than";
        case CompareOpType::GREATER_EQUAL:
          return "Greater equal";
        case CompareOpType::IS:
          return "Is";
        case CompareOpType::IS_NOT:
          return "Is not";
        case CompareOpType::IN:
          return "In";
        case CompareOpType::NOT_IN:
          return "Not in";
      }
      return "";
    }());
  };
  AppendList("ops", node->ops, this, print_op);
  Append(",");

  AppendList("comparators", node->comparators, this);
  Append(")");
  indentation -= 1;
}

void DebugStringVisitor::Visit(Constant* node) {
  Append("Constant(value=");
  Append(ConstantValueString(node->value));
  Append(")");
}

void DebugStringVisitor::Visit(Name* node) {
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