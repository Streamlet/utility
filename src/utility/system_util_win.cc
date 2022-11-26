#include "system_util.h"
#include <memory>
#include <Windows.h>

namespace system_util {

std::string GetPathSep() {
  return "\\";
}

std::string GetTempDirPath() {
  std::string path;
  path.resize(MAX_PATH);
  DWORD len = ::GetTempPathA(MAX_PATH, path.data());
  path.resize(len);
  return std::move(path);
}
}
