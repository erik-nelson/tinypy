load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

cc_library(
  name = "interpreter",
  srcs = ["interpreter.cc"],
  hdrs = ["interpreter.h"],
  deps = [
    ":lexer",
    ":parser",
    ":stream",
    ":token",
  ],
)

cc_library(
  name = "lexer",
  srcs = ["lexer.cc"],
  hdrs = ["lexer.h"],
  deps = [
    ":stream",
    ":token",
  ],
)

cc_test(
  name = "lexer_test",
  srcs = ["lexer_test.cc"],
  deps = [
    ":lexer",
    ":token",
    "@gtest//:gtest_main",
  ],
)

cc_library(
  name = "parser",
  srcs = ["parser.cc"],
  hdrs = ["parser.h"],
  deps = [
    ":stream",
    ":syntax_tree",
    ":token",
  ],
)

cc_binary(
  name = "repl",
  srcs = ["repl.cc"],
  deps = [
    ":interpreter",
    ":version",
  ],
)

cc_library(
  name = "stream",
  hdrs = ["stream.h"],
)

cc_test(
  name = "stream_test",
  srcs = ["stream_test.cc"],
  deps = [
    ":stream",
    "@gtest//:gtest_main",
  ],
)


cc_library(
  name = "syntax_tree",
  srcs = [
    "syntax_tree.cc",
    "syntax_tree_node.cc",
    "syntax_tree_visitor.cc",
  ],
  hdrs = [
    "syntax_tree.h",
    "syntax_tree_node.h",
    "syntax_tree_visitor.h",  
  ],
  deps = [":types"],
)

cc_test(
  name = "syntax_tree_test",
  srcs = ["syntax_tree_test.cc"],
  deps = [
    ":lexer",
    ":parser",
    ":syntax_tree",
    "@gtest//:gtest_main",
  ],
)

cc_library(
  name = "token",
  hdrs = ["token.h"],
  srcs = ["token.cc"],
)

cc_library(
  name = "types",
  hdrs = ["types.h"],
)

cc_library(
  name = "version",
  hdrs = ["version.h"],
  srcs = ["version.cc"],
)