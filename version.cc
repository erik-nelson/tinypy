#include "version.h"
#include <sstream>

/*static*/ std::string VersionInfo::ToString() {
  std::ostringstream ss;
  ss << kMajor << "." << kMinor << "." << kPatch;
  return ss.str();
}