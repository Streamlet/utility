
#include "../string/native_string.h"
#include "process_util.h"
#include <cstdio>

#if defined(_WIN32)
#define SHELL_CMD _T("cmd")
#define SHELL_SWITCH _T("/C")
#define PING_CMD_10S _T("ping -n 10 127.0.0.1")
#define PING_CMD_20S _T("ping -n 20 127.0.0.1")
#elif defined(__APPLE__)
#define SHELL_CMD _T("bash")
#define SHELL_SWITCH _T("-c")
#define PING_CMD_10S _T("ping -t 10 127.0.0.1")
#define PING_CMD_20S _T("ping -t 20 127.0.0.1")
#else
#define SHELL_CMD _T("sh")
#define SHELL_SWITCH _T("-c")
#define PING_CMD_10S _T("ping -w 10 127.0.0.1")
#define PING_CMD_20S _T("ping -w 20 127.0.0.1")
#endif

int _tmain(int argc, const TCHAR *argv[]) {
  native_string exe_path = process_util::GetExecutablePath();
  _tprintf(_T("Exe Path: %s\n"), exe_path.c_str());

  long pid = process_util::StartProcess(SHELL_CMD, {SHELL_SWITCH, PING_CMD_10S});
  _tprintf(_T("New Process for 10s: %ld\n"), pid);

  _tprintf(_T("Waiting 20s\n"));
  bool waited = process_util::WaitProcess(pid, 20000);
  _tprintf(_T("Waited 20s: %d\n"), (int)waited);

  pid = process_util::StartProcess(SHELL_CMD, {SHELL_SWITCH, PING_CMD_20S});
  _tprintf(_T("New Process for 20s: %ld\n"), pid);

  _tprintf(_T("Waiting 5s\n"));
  waited = process_util::WaitProcess(pid, 5000);
  _tprintf(_T("Waited 5s: %d\n"), (int)waited);

  bool terminated = process_util::KillProcess(pid);
  _tprintf(_T("Terminated: %d\n"), (int)terminated);

  _tprintf(_T("Sleep 5s\n"));
  process_util::Sleep(5000);

  _tprintf(_T("OK\n"));
  return 0;
}
