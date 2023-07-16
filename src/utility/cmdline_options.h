#pragma once

// Boost.ProgramOptions is too large, so implement an easy one here

#include <map>
#include <memory>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <tchar.h>
#else
#define TCHAR char
#endif

namespace cmdline_options {

struct ParsedOption {
  std::map<std::basic_string<TCHAR>, std::basic_string<TCHAR>> parsed_map;

  bool has(const TCHAR *option_name) const {
    return parsed_map.find(option_name) != parsed_map.end();
  }

  std::basic_string<TCHAR> get(const TCHAR *option_name) const {
    auto it = parsed_map.find(option_name);
    return it != parsed_map.end() ? it->second : std::basic_string<TCHAR>();
  }

  template <typename T>
  T get_as(const TCHAR *option_name) const {
    std::basic_stringstream<TCHAR> ss;
    ss << get(option_name);
    T t = {};
    ss >> t;
    return std::move(t);
  }
};

ParsedOption parse(int argc, const TCHAR *argv[]);

#ifdef _WIN32
ParsedOption parse(const TCHAR *cmdline);
#endif

} // namespace cmdline_options
