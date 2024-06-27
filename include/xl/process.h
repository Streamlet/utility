#pragma once

#include "native_string.h"
#include <string>
#include <vector>

namespace xl {

namespace process {

native_string executable_path();

long pid();
long tid();

long start(const native_string &executable,
           const std::vector<native_string> &arguments,
           const native_string &start_dir = {},
           unsigned int milliseconds = 0);

// milliseconds: 0 means not wait; -1 means infinate; otherwise wait for 'milliseconds' milliseconds
bool wait(long pid, unsigned int milliseconds = -1);

bool kill(long pid);

void sleep(unsigned int milliseconds);

} // namespace process

} // namespace xl
