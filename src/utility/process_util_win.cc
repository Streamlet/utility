#include "native_string.h"
#include "process_util.h"
#include <Loki/ScopeGuard.h>
#include <sstream>
#include <Windows.h>

namespace process_util {

native_string GetExecutablePath() {
  TCHAR path[MAX_PATH] = {0};
  DWORD length = ::GetModuleFileName(NULL, path, MAX_PATH);
  DWORD last_error = ::GetLastError();
  if (last_error == ERROR_SUCCESS && length < MAX_PATH)
    return path;
  if (!(last_error == ERROR_SUCCESS && length > MAX_PATH || last_error == ERROR_INSUFFICIENT_BUFFER))
    return _T("");
  native_string long_path(last_error == ERROR_SUCCESS ? length : 32768, _T('\0'));
  length = ::GetModuleFileName(NULL, &long_path[0], (DWORD)long_path.length());
  if (last_error != ERROR_SUCCESS)
    return _T("");
  long_path.resize(length);
  return std::move(long_path);
}

long GetPid() {
  return ::GetCurrentProcessId();
}

namespace {

native_string quote(const native_string &s) {
  if (s.empty() || s.find(_T(' ')) == s.npos || (*s.begin() == _T('"') && *s.rbegin() == _T('"')))
    return s;
  return _T("\"") + s + _T("\"");
}

} // namespace

long StartProcess(const native_string &executable,
                  const std::vector<native_string> &arguments,
                  const native_string &start_dir,
                  unsigned int milliseconds) {
  native_string_stream ss;
  ss << std::move(quote(executable));
  for (const auto &argument : arguments) {
    ss << _T(" ") << quote(argument);
  }
  native_string command_line = ss.str();

  STARTUPINFO si = {sizeof(STARTUPINFO)};
  PROCESS_INFORMATION pi = {};
  if (::CreateProcess(NULL, &command_line[0], NULL, NULL, FALSE, 0, NULL, start_dir.empty() ? NULL : start_dir.c_str(),
                      &si, &pi) == 0) {
    return 0;
  }
  if (milliseconds > 0)
    ::WaitForSingleObject(pi.hProcess, milliseconds);

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  return pi.dwProcessId;
}

bool WaitProcess(long pid, unsigned int milliseconds) {
  HANDLE hProcess = ::OpenProcess(SYNCHRONIZE, FALSE, (DWORD)pid);
  if (hProcess == NULL)
    return true;
  LOKI_ON_BLOCK_EXIT(::CloseHandle, hProcess);
  if (::WaitForSingleObject(hProcess, milliseconds) == WAIT_OBJECT_0)
    return true;
  return false;
}

bool KillProcess(long pid) {
  HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, (DWORD)pid);
  if (hProcess == NULL)
    return false;
  LOKI_ON_BLOCK_EXIT(::CloseHandle, hProcess);
  if (!::TerminateProcess(hProcess, -1))
    return false;
  return true;
}

void Sleep(unsigned int milliseconds) {
  ::Sleep(milliseconds);
}

} // namespace process_util
