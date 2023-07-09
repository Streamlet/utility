
#include "cmdline_options.h"
#include <sstream>
#include <stdio.h>
#include <windows.h>

int WINAPI wWinMain(__in HINSTANCE hInstance,
                    __in_opt HINSTANCE hPrevInstance,
                    __in LPTSTR lpCmdLine,
                    __in int nShowCmd) {
  LPCTSTR cmdline = ::GetCommandLine();
  std::wstringstream ss;
  ss << L"Original: " << cmdline << L"\n";
  auto options = cmdline_options::parse(cmdline);
  for (const auto &item : options) {
    ss << item.first << L": " << item.second << L"\n";
  }
  std::wstring str = ss.str();
  ::MessageBox(NULL, str.c_str(), L"Arguments", 0);
  return 0;
}
