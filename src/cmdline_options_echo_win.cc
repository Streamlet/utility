
#include "cmdline_options.h"
#include "native_string.h"
#include <cstdio>
#include <sstream>
#include <windows.h>

int WINAPI _tWinMain(__in HINSTANCE hInstance,
                     __in_opt HINSTANCE hPrevInstance,
                     __in LPTSTR lpCmdLine,
                     __in int nShowCmd) {
  LPCTSTR cmdline = ::GetCommandLine();
  xl::native_string_stream ss;
  ss << _T("Original: ") << cmdline << _T("\n");
  auto options = xl::cmdline_options::parse(cmdline);
  for (const auto &item : options.parsed_map) {
    ss << item.first << _T(": ") << item.second << _T("\n");
  }
  std::basic_string<TCHAR> str = ss.str();
  ::MessageBox(NULL, str.c_str(), _T("Arguments"), 0);
  return 0;
}
