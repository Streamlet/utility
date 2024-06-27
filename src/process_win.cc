#include <sstream>
#include <Windows.h>
#include <xl/native_string>
#include <xl/process>
#include <xl/scope_exit>

namespace xl {

namespace process {

native_string executable_path() {
  TCHAR path[MAX_PATH] = {0};
  DWORD length = ::GetModuleFileName(NULL, path, MAX_PATH);
  if (length < MAX_PATH) {
    return path;
  }
  DWORD last_error = ::GetLastError();
  if (!(length > MAX_PATH || last_error == ERROR_INSUFFICIENT_BUFFER)) {
    return _T("");
  }
  native_string long_path(last_error == ERROR_SUCCESS ? length : 32768, _T('\0'));
  length = ::GetModuleFileName(NULL, &long_path[0], (DWORD)long_path.length());
  if (last_error != ERROR_SUCCESS) {
    return _T("");
  }
  long_path.resize(length);
  return std::move(long_path);
}

long pid() {
  return ::GetCurrentProcessId();
}

long tid() {
  return ::GetCurrentThreadId();
}

namespace {

native_string quote(const native_string &s) {
  if (s.empty() || s.find(_T(' ')) == s.npos || (*s.begin() == _T('"') && *s.rbegin() == _T('"'))) {
    return s;
  }
  return _T("\"") + s + _T("\"");
}

} // namespace

long start(const native_string &executable,
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
  if (milliseconds > 0) {
    ::WaitForSingleObject(pi.hProcess, milliseconds);
  }

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  return pi.dwProcessId;
}

bool wait(long pid, unsigned int milliseconds) {
  HANDLE hProcess = ::OpenProcess(SYNCHRONIZE, FALSE, (DWORD)pid);
  if (hProcess == NULL) {
    return true;
  }
  XL_ON_BLOCK_EXIT(::CloseHandle, hProcess);
  if (::WaitForSingleObject(hProcess, milliseconds) == WAIT_OBJECT_0) {
    return true;
  }
  return false;
}

bool kill(long pid) {
  HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, (DWORD)pid);
  if (hProcess == NULL) {
    return false;
  }
  XL_ON_BLOCK_EXIT(::CloseHandle, hProcess);
  if (!::TerminateProcess(hProcess, -1)) {
    return false;
  }
  return true;
}

void sleep(unsigned int milliseconds) {
  ::Sleep(milliseconds);
}

} // namespace process

} // namespace xl
