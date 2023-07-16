
#include "cmdline_options.h"
#include "native_string.h"
#include <stdio.h>

int _tmain(int argc, const TCHAR *argv[]) {
  auto options = cmdline_options::parse(argc, argv);
  for (const auto &item : options.parsed_map) {
    _tprintf(_T("%s: %s\n"), item.first.c_str(), item.second.c_str());
  }
  return 0;
}
