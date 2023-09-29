load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

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
    name = "version",
    hdrs = ["version.h"],
    srcs = ["version.cc"],
)

cc_library(
    name = "token",
    hdrs = ["token.h"],
    srcs = ["token.cc"],
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
    name = "interpreter",
    srcs = ["interpreter.cc"],
    hdrs = ["interpreter.h"],
    deps = [":lexer"],
)

cc_binary(
    name = "repl",
    srcs = ["repl.cc"],
    deps = [
        ":interpreter",
        ":version",
    ],
)