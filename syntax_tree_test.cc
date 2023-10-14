
#include "syntax_tree.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "lexer.h"
#include "parser.h"

SyntaxTree BuildSyntaxTree(std::string source,
                           Parser::Mode mode = Parser::Mode::MODULE) {
  Lexer lexer(std::move(source));
  Parser parser(lexer.TokenStream(), mode);
  parser.Parse();
  return std::move(parser).syntax_tree();
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
                Name(id='a', ctx=Del),
                Name(id='Foo', ctx=Del),
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
                Name(id='a', ctx=Store),
                Name(id='b', ctx=Store)],
            value=BinaryOp(
                lhs=Name(id='c', ctx=Load),
                op=Add,
                rhs=Constant(value=Int: 5)))])
)");

  DebugPrint(source, tree);
}

TEST(SyntaxTree, Compare) {
  std::string source = "a < 5";
  SyntaxTree tree = BuildSyntaxTree(source);

  DebugStringVisitor visitor;
  tree.Traverse(&visitor);
  EXPECT_EQ(visitor.str,
            R"(Module(
    body=[
        Expr(
            value=Compare(
                lhs=Name(id='a', ctx=Load),
                ops=[
                    Less than],
                comparators=[
                    Constant(value=Int: 5)]))])
)");

  DebugPrint(source, tree);
}

TEST(SyntaxTree, MultiCompare) {
  std::string source =
      "a == b != c < d <= e > f >= g is h is not i in j not in k";
  SyntaxTree tree = BuildSyntaxTree(source);

  DebugStringVisitor visitor;
  tree.Traverse(&visitor);

  EXPECT_EQ(visitor.str,
            R"(Module(
    body=[
        Expr(
            value=Compare(
                lhs=Name(id='a', ctx=Load),
                ops=[
                    Equals,
                    Not equals,
                    Less than,
                    Less equal,
                    Greater than,
                    Greater equal,
                    Is,
                    Is not,
                    In,
                    Not in],
                comparators=[
                    Name(id='b', ctx=Load),
                    Name(id='c', ctx=Load),
                    Name(id='d', ctx=Load),
                    Name(id='e', ctx=Load),
                    Name(id='f', ctx=Load),
                    Name(id='g', ctx=Load),
                    Name(id='h', ctx=Load),
                    Name(id='i', ctx=Load),
                    Name(id='j', ctx=Load),
                    Name(id='k', ctx=Load)]))])
)");

  DebugPrint(source, tree);
}

TEST(SyntaxTree, IfSingleLine) {
  std::string source = "if a: b";
  SyntaxTree tree = BuildSyntaxTree(source);

  DebugStringVisitor visitor;
  tree.Traverse(&visitor);
  EXPECT_EQ(visitor.str,
            R"(Module(
    body=[
        If(
            test=Name(id='a', ctx=Load),
            then=[
                Expr(
                    value=Name(id='b', ctx=Load))],
            else=[])])
)");

  DebugPrint(source, tree);
}

TEST(SyntaxTree, IfMultiLine) {
    std::string source = 
R"(
if a:
    b
)";

  SyntaxTree tree = BuildSyntaxTree(source);

  DebugStringVisitor visitor;
  tree.Traverse(&visitor);
  EXPECT_EQ(visitor.str,
            R"(Module(
    body=[
        If(
            test=Name(id='a', ctx=Load),
            then=[
                Expr(
                    value=Name(id='b', ctx=Load))],
            else=[])])
)");

  DebugPrint(source, tree);
}

TEST(SyntaxTree, IfElse) {
    std::string source = 
R"(
if a:
    b
else:
    c
)";

  SyntaxTree tree = BuildSyntaxTree(source);

  DebugStringVisitor visitor;
  tree.Traverse(&visitor);
  EXPECT_EQ(visitor.str, R"(Module(
    body=[
        If(
            test=Name(id='a', ctx=Load),
            then=[
                Expr(
                    value=Name(id='b', ctx=Load))],
            else=[
                Expr(
                    value=Name(id='c', ctx=Load))])])
)");

  DebugPrint(source, tree);
}

TEST(SyntaxTree, IfElif) {
    std::string source = 
R"(
if a:
    b
elif c:
    d
else:
    e
)";

  SyntaxTree tree = BuildSyntaxTree(source);

  DebugStringVisitor visitor;
  tree.Traverse(&visitor);
  EXPECT_EQ(visitor.str, R"(Module(
    body=[
        If(
            test=Name(id='a', ctx=Load),
            then=[
                Expr(
                    value=Name(id='b', ctx=Load))],
            else=[
                If(
                    test=Name(id='c', ctx=Load),
                    then=[
                        Expr(
                            value=Name(id='d', ctx=Load))],
                    else=[
                        Expr(
                            value=Name(id='e', ctx=Load))])])])
)");

  DebugPrint(source, tree);
}

#if 0
TEST(SyntaxTree, NestedIfElse) {
    std::string source = 
R"(
if a:
    if b:
        c
    elif d:
        e
    else:
        g
elif h:
    if i:
        j
    elif k:
        l
    else:
        m
else:
    if n:
        o
    elif p:
        q
    else:
        r
)";

  SyntaxTree tree = BuildSyntaxTree(source);

  DebugStringVisitor visitor;
  tree.Traverse(&visitor);
  EXPECT_EQ(visitor.str, R"(Module(
    body=[
        If(
            test=Name(id='a', ctx=Load),
            then=[
                If(
                    test=Name(id='b', ctx=Load),
                    then=[
                        Expr(
                            value=Name(id='c', ctx=Load))],
                    else=[
                        If(
                            test=Name(id='d', ctx=Load),
                            then=[
                                Expr(
                                    value=Name(id='e', ctx=Load))],
                            else=[
                                Expr(
                                    value=Name(id='g', ctx=Load))])])],
            else=[
                If(
                    test=Name(id='h', ctx=Load),
                    then=[
                        If(
                            test=Name(id='i', ctx=Load),
                            then=[
                                Expr(
                                    value=Name(id='j', ctx=Load))],
                            else=[
                                If(
                                    test=Name(id='k', ctx=Load),
                                    then=[
                                        Expr(
                                            value=Name(id='l', ctx=Load))],
                                    else=[
                                        Expr(
                                            value=Name(id='m', ctx=Load))])])],
                    else=[
                        If(
                            test=Name(id='n', ctx=Load),
                            then=[
                                Expr(
                                    value=Name(id='o', ctx=Load))],
                            else=[
                                If(
                                    test=Name(id='p', ctx=Load),
                                    then=[
                                        Expr(
                                            value=Name(id='q', ctx=Load))],
                                    else=[
                                        Expr(
                                            value=Name(id='r', ctx=Load))])])])])])
)");

  DebugPrint(source, tree);
}
#endif