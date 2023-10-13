
#include "syntax_tree.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "lexer.h"
#include "parser.h"

SyntaxTree BuildSyntaxTree(std::string source,
                           Parser::Mode mode = Parser::Mode::MODULE) {
  Lexer lexer(std::move(source));
  return Parser(lexer.TokenStream(), mode).Parse();
}

void DebugPrint(const std::string& source, const SyntaxTree& tree) {
  std::cout << "---------- Source code ----------\n"
            << source << "\n\n---------- Syntax tree ----------\n"
            << tree << "\n";
}

TEST(SyntaxTree, ParserModes) {
  std::string source = "'hello, world!'";
  {
    SyntaxTree tree = BuildSyntaxTree(source, Parser::Mode::MODULE);
    DebugStringVisitor visitor;
    tree.Traverse(&visitor);
    EXPECT_EQ(visitor.str,
              R"(Module(
    body=[
        Expr(
            value=Constant(value=String: 'hello, world!'))])
)");
    DebugPrint(source, tree);
  }

  {
    SyntaxTree tree = BuildSyntaxTree(source, Parser::Mode::INTERACTIVE);
    DebugStringVisitor visitor;
    tree.Traverse(&visitor);
    EXPECT_EQ(visitor.str,
              R"(Interactive(
    body=[
        Expr(
            value=Constant(value=String: 'hello, world!'))])
)");
    DebugPrint(source, tree);
  }

  {
    SyntaxTree tree = BuildSyntaxTree(source, Parser::Mode::EXPRESSION);
    DebugStringVisitor visitor;
    tree.Traverse(&visitor);
    EXPECT_EQ(visitor.str,
              R"(Expression(
    body=Constant(value=String: 'hello, world!'))
)");
    DebugPrint(source, tree);
  }
}

TEST(SyntaxTree, BinaryAddition) {
  std::string source = "3 + 5";
  SyntaxTree tree = BuildSyntaxTree(source);

  DebugStringVisitor visitor;
  tree.Traverse(&visitor);
  EXPECT_EQ(visitor.str,
            R"(Module(
    body=[
        Expr(
            value=BinaryOp(
                lhs=Constant(value=Int: 3),
                op=Add,
                rhs=Constant(value=Int: 5)))])
)");

  DebugPrint(source, tree);
}

TEST(SyntaxTree, Delete) {
  std::string source = "del a, Foo, bar";

  SyntaxTree tree = BuildSyntaxTree(source);

  DebugStringVisitor visitor;
  tree.Traverse(&visitor);
  EXPECT_EQ(visitor.str,
            R"(Module(
    body=[
        Delete(
            targets=[
                Name(id='a', ctx=Del)
                Name(id='Foo', ctx=Del)
                Name(id='bar', ctx=Del)])])
)");

  DebugPrint(source, tree);
}

TEST(SyntaxTree, AssignSingle) {
  std::string source = "a = 5";
  SyntaxTree tree = BuildSyntaxTree(source);

  DebugStringVisitor visitor;
  tree.Traverse(&visitor);
  EXPECT_EQ(visitor.str,
            R"(Module(
    body=[
        Assign(
            targets=[
                Name(id='a', ctx=Store)],
            value=Constant(value=Int: 5))])
)");

  DebugPrint(source, tree);
}

TEST(SyntaxTree, AssignMulti) {
  std::string source = "a = b = c + 5";
  SyntaxTree tree = BuildSyntaxTree(source);

  DebugStringVisitor visitor;
  tree.Traverse(&visitor);
  EXPECT_EQ(visitor.str,
            R"(Module(
    body=[
        Assign(
            targets=[
                Name(id='a', ctx=Store)
                Name(id='b', ctx=Store)],
            value=BinaryOp(
                lhs=Name(id='c', ctx=Load),
                op=Add,
                rhs=Constant(value=Int: 5)))])
)");

  DebugPrint(source, tree);
}