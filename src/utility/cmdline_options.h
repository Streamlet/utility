#pragma once

// Boost.ProgramOptions is too large, so implement an easy one here

#include <map>
#include <memory>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <tchar.h>
#endif

namespace cmdline_options {

template <typename CharType>
struct ParsedOption {
  std::map<std::basic_string<CharType>, std::basic_string<CharType>> parsed_map;

  bool has(const CharType *option_name) {
    return parsed_map.find(option_name) != parsed_map.end();
  }

  std::basic_string<CharType> get(const CharType *option_name) {
    auto it = parsed_map.find(option_name);
    return it != parsed_map.end() ? it->second : std::basic_string<CharType>();
  }

  template <typename T>
  T get_as(const CharType *option_name) {
    std::basic_stringstream<CharType> ss;
    ss << get(option_name);
    T t = {};
    ss >> t;
    return std::move(t);
  }
};

#ifdef _WIN32
ParsedOption<TCHAR> parse(int argc, const TCHAR *argv[]);
ParsedOption<TCHAR> parse(const TCHAR *cmdline);
#else
ParsedOption<char> parse(int argc, const char *argv[]);
#endif

} // namespace cmdline_options
