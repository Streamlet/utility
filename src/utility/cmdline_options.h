#pragma once

// Boost.ProgramOptions is too large, so implement an easy one here

#include <map>
#include <string>

namespace cmdline_options {

std::map<std::string, std::string> parse(int argc, const char *argv[]);
std::map<std::wstring, std::wstring> parse(int argc, const wchar_t *argv[]);

#ifdef _WIN32
std::map<std::string, std::string> parse(const char *cmdline);
std::map<std::wstring, std::wstring> parse(const wchar_t *cmdline);
#endif

} // namespace cmdline_options
