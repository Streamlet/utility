// MIT License
//
// Copyright (c) 2022 Streamlet (streamlet@outlook.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cstdio>
#include <xl/native_string>
#include <xl/process>

#if defined(_WIN32)
#define SHELL_CMD    _T("cmd")
#define SHELL_SWITCH _T("/C")
#define PING_CMD_10S _T("ping -n 10 127.0.0.1")
#define PING_CMD_20S _T("ping -n 20 127.0.0.1")
#elif defined(__APPLE__)
#define SHELL_CMD    _T("bash")
#define SHELL_SWITCH _T("-c")
#define PING_CMD_10S _T("ping -t 10 127.0.0.1")
#define PING_CMD_20S _T("ping -t 20 127.0.0.1")
#else
#define SHELL_CMD    _T("sh")
#define SHELL_SWITCH _T("-c")
#define PING_CMD_10S _T("ping -w 10 127.0.0.1")
#define PING_CMD_20S _T("ping -w 20 127.0.0.1")
#endif

int _tmain(int argc, const TCHAR *argv[]) {
  xl::native_string exe_path = xl::process::executable_path();
  _tprintf(_T("Exe Path: %s\n"), exe_path.c_str());

  long pid = xl::process::start(SHELL_CMD, {SHELL_SWITCH, PING_CMD_10S});
  _tprintf(_T("New Process for 10s: %ld\n"), pid);

  _tprintf(_T("Waiting 20s\n"));
  bool waited = xl::process::wait(pid, 20000);
  _tprintf(_T("Waited 20s: %d\n"), (int)waited);

  pid = xl::process::start(SHELL_CMD, {SHELL_SWITCH, PING_CMD_20S});
  _tprintf(_T("New Process for 20s: %ld\n"), pid);

  _tprintf(_T("Waiting 5s\n"));
  waited = xl::process::wait(pid, 5000);
  _tprintf(_T("Waited 5s: %d\n"), (int)waited);

  bool terminated = xl::process::kill(pid);
  _tprintf(_T("Terminated: %d\n"), (int)terminated);

  _tprintf(_T("Sleep 5s\n"));
  xl::process::sleep(5000);

  _tprintf(_T("OK\n"));
  return 0;
}
