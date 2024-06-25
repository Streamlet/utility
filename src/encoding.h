#pragma once

#include <string>

namespace xl {

namespace encoding {

std::wstring utf8_to_utf16(const char *utf8, size_t length);
std::wstring utf8_to_utf16(const char *utf8);
std::wstring utf8_to_utf16(const std::string &utf8);

std::string utf16_to_utf8(const wchar_t *utf16, size_t length);
std::string utf16_to_utf8(const wchar_t *utf16);
std::string utf16_to_utf8(const std::wstring &utf16);

#ifdef _WIN32

std::wstring ansi_to_utf16(const char *ansi, size_t length);
std::wstring ansi_to_utf16(const char *ansi);
std::wstring ansi_to_utf16(const std::string &ansi);

std::string utf16_to_ansi(const wchar_t *ucs2, size_t length);
std::string utf16_to_ansi(const wchar_t *ucs2);
std::string utf16_to_ansi(const std::wstring &ucs2);

#endif

} // namespace encoding

} // namespace xl
