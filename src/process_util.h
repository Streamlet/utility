#pragma once

#include <string>
#include <vector>

#ifdef _WIN32
#include <tchar.h>
#else
#define TCHAR char
#endif

namespace process_util {

std::basic_string<TCHAR> GetExecutablePath();

long GetPid();
long GetTid();

long StartProcess(const std::basic_string<TCHAR> &executable,
                  const std::vector<std::basic_string<TCHAR>> &arguments,
                  const std::basic_string<TCHAR> &start_dir = {},
                  unsigned int milliseconds = 0);

// milliseconds: 0 means not wait; -1 means infinate; otherwise wait for 'milliseconds' milliseconds
bool WaitProcess(long pid, unsigned int milliseconds = -1);

bool KillProcess(long pid);

void Sleep(unsigned int milliseconds);

} // namespace process_util
