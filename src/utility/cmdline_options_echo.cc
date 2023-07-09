
#include "cmdline_options.h"
#include <stdio.h>

int main(int argc, const char *argv[]) {
  auto options = cmdline_options::parse(argc, argv);
  for (const auto &item : options) {
    printf("%s: %s\n", item.first.c_str(), item.second.c_str());
  }
  return 0;
}
