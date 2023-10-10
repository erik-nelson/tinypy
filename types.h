#pragma once

#include <string>
#include <variant>

// An identifier for a python variable, function, or class.
using Identifier = std::string;

// A python constant value can be one of the following types.
// TODO(erik): Immutable container types (tuples, frozenset).
struct NoneType {};
using ConstantValue = std::variant<std::string, int, double, bool, NoneType>;