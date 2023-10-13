#pragma once

#include <string>
#include <variant>

// An identifier for a python variable, function, or class.
using Identifier = std::string;

// A python constant value can be one of the following types.
// TODO(erik): Immutable container types (tuples, frozenset).
struct NoneType {};
using ConstantValue = std::variant<std::string, int, double, bool, NoneType>;

// Statically defined python object types. Used in object.h. More types can be
// defined on the fly, but these enumerate the built-in types.
enum class StaticObjectType {
  NONE = 0,
  INT,
  FLOAT,
  STRING,
  BOOL,
  LIST,
  TUPLE,
  DICT,
  SET,
  FUNCTION,
  CLASS,
  NUM_TYPES,
};

// An object type is a thin wrapper around an int, which holds -1 (invalid
// type) as a default.
struct Type {
  Type() = default;
  Type(int type) : type(type) {}
  Type(StaticObjectType type) : type(static_cast<int>(type)) {}
  bool operator==(Type rhs) const { return type == rhs.type; }
  bool operator!=(Type rhs) const { return !(*this == rhs); }
  operator int() const { return this->type; }
  int type = -1;
};