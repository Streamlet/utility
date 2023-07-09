
#include "cmdline_options.h"
#include <cstring>
#include <cwchar>
#include <loki/ScopeGuard.h>
#ifdef _WIN32
#include "encoding.h"
#include <shellapi.h>
#include <windows.h>
#endif

namespace cmdline_options {

namespace {

template <typename CharType>
std::map<std::basic_string<CharType>, std::basic_string<CharType>> parseT(int argc, const CharType *argv[]) {
  typedef std::basic_string<CharType> string_type;
  std::map<std::basic_string<CharType>, std::basic_string<CharType>> result;
  for (int i = 1; i < argc; ++i) {
    string_type arg = argv[i];
    if (arg.length() >= 2 && arg[0] == (CharType)'-' && arg[1] == (CharType)'-') {
      auto equal_pos = arg.find_first_of((CharType)'=', 2);
      if (equal_pos != string_type::npos) {
        string_type k = arg.substr(2, equal_pos - 2);
        string_type v = arg.substr(equal_pos + 1);
        result.insert(std::make_pair(k, v));
      } else {
        string_type k = arg.substr(2);
        result.insert(std::make_pair(k, string_type()));
      }
    } else if (arg.length() >= 1 && arg[0] == (CharType)'-') {
      string_type k = arg.substr(1);
      if (i + 1 < argc && argv[i + 1][0] != (CharType)'-') {
        result.insert(std::make_pair(k, argv[i + 1]));
        ++i;
      } else {
        result.insert(std::make_pair(k, string_type()));
      }
    } else {
      result.insert(std::make_pair(arg, string_type()));
    }
  }
  return std::move(result);
}

} // namespace

std::map<std::string, std::string> parse(int argc, const char *argv[]) {
  return parseT(argc, argv);
}

std::map<std::wstring, std::wstring> parse(int argc, const wchar_t *argv[]) {
  return parseT(argc, argv);
}

#ifdef _WIN32

std::map<std::string, std::string> parse(const char *cmdline) {
  std::wstring wstr = encoding::ANSIToUCS2(cmdline);
  std::map<std::wstring, std::wstring> wresult = parse(wstr.c_str());
  std::map<std::string, std::string> result;
  for (const auto &item : wresult) {
    result.insert(std::make_pair(encoding::UCS2ToANSI(item.first), encoding::UCS2ToANSI(item.second)));
  }
  return std::move(result);
}

std::map<std::wstring, std::wstring> parse(const wchar_t *cmdline) {
  int argc = 0;
  LPWSTR *argv = ::CommandLineToArgvW(cmdline, &argc);
  if (argv == NULL)
    return {};
  LOKI_ON_BLOCK_EXIT(::LocalFree, argv);
  return parseT(argc, (LPCWSTR *)argv);
}

#endif

} // namespace cmdline_options