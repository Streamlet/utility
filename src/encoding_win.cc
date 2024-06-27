#include <Windows.h>
#include <xl/encoding.h>

namespace xl {

namespace encoding {

namespace {

std::wstring a2w(const char *ansi, size_t length, UINT code_page) {
  std::wstring ucs2;
  int size = ::MultiByteToWideChar(code_page, 0, ansi, (int)length, nullptr, 0);
  if (size == 0) {
    return ucs2;
  }
  ucs2.resize(length == -1 || ansi[length - 1] == '\0' ? size - 1 : size);
  ::MultiByteToWideChar(code_page, 0, ansi, (int)length, &ucs2[0], size);
  return ucs2;
}

std::string w2a(const wchar_t *ucs2, size_t length, UINT code_page) {
  std::string ansi;
  int size = ::WideCharToMultiByte(code_page, 0, ucs2, (int)length, nullptr, 0, nullptr, nullptr);
  if (size == 0) {
    return ansi;
  }
  ansi.resize(length == -1 || ucs2[length - 1] == L'\0' ? size - 1 : size);
  ::WideCharToMultiByte(code_page, 0, ucs2, (int)length, &ansi[0], size, nullptr, nullptr);
  return ansi;
}

} // namespace

std::wstring utf8_to_utf16(const char *utf8, size_t length) {
  return a2w(utf8, length, CP_UTF8);
}

std::wstring utf8_to_utf16(const char *utf8) {
  return utf8_to_utf16(utf8, -1);
}

std::wstring utf8_to_utf16(const std::string &utf8) {
  return utf8_to_utf16(utf8.c_str(), utf8.length());
}

std::string utf16_to_utf8(const wchar_t *ucs2, size_t length) {
  return w2a(ucs2, length, CP_UTF8);
}

std::string utf16_to_utf8(const wchar_t *ucs2) {
  return utf16_to_utf8(ucs2, -1);
}

std::string utf16_to_utf8(const std::wstring &ucs2) {
  return utf16_to_utf8(ucs2.c_str(), ucs2.length());
}

std::wstring ansi_to_utf16(const char *ansi, size_t length) {
  return a2w(ansi, length, CP_ACP);
}

std::wstring ansi_to_utf16(const char *ansi) {
  return ansi_to_utf16(ansi, -1);
}

std::wstring ansi_to_utf16(const std::string &ansi) {
  return ansi_to_utf16(ansi.c_str(), ansi.length());
}

std::string utf16_to_ansi(const wchar_t *ucs2, size_t length) {
  return w2a(ucs2, length, CP_ACP);
}

std::string utf16_to_ansi(const wchar_t *ucs2) {
  return utf16_to_ansi(ucs2, -1);
}

std::string utf16_to_ansi(const std::wstring &ucs2) {
  return utf16_to_ansi(ucs2.c_str(), ucs2.length());
}

} // namespace encoding

} // namespace xl
