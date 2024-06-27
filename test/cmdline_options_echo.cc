
#include <cstdio>
#include <xl/cmdline_options.h>
#include <xl/native_string.h>

int _tmain(int argc, const TCHAR *argv[]) {
  auto options = xl::cmdline_options::parse(argc, argv);
  for (const auto &item : options.parsed_map) {
    _tprintf(_T("%s: %s\n"), item.first.c_str(), item.second.c_str());
  }
  return 0;
}
