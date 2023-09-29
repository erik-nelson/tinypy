#pragma once

#include <string>

struct VersionInfo {
  static constexpr size_t kMajor = 0;
  static constexpr size_t kMinor = 0;
  static constexpr size_t kPatch = 0;
  static std::string ToString();
};