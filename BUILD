load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

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
    deps = [":token"],
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