#include "encoding.h"

namespace encoding {

std::wstring UTF8ToUCS2(const std::string &utf8) {
  return UTF8ToUCS2(utf8.c_str(), utf8.length());
}

std::wstring UTF8ToUCS2(const std::string_view &utf8) {
  return UTF8ToUCS2(utf8.data(), utf8.length());
}

std::wstring UTF8ToUCS2(const char *utf8) { return UTF8ToUCS2(utf8, -1); }

std::wstring UTF8ToUCS2(const char *utf8, size_t length) {
  std::wstring result;
  int size = ::MultiByteToWideChar(CP_UTF8, 0, utf8, length, nullptr, 0);
  if (size == 0)
    return result;
  result.resize(size);
  ::MultiByteToWideChar(CP_UTF8, 0, utf8, length, result.data(), size);
  return result;
}

std::string UCS2ToUTF8(const std::wstring &ucs2) {
  return UCS2ToUTF8(ucs2.c_str(), ucs2.length());
}

std::string UCS2ToUTF8(const std::wstring_view &ucs2) {
  return UCS2ToUTF8(ucs2.data(), ucs2.length());
}

std::string UCS2ToUTF8(const wchar_t *ucs2) { return UCS2ToUTF8(ucs2, -1); }

std::string UCS2ToUTF8(const wchar_t *ucs2, size_t length) {
  std::string result;
  int size = ::WideCharToMultiByte(CP_UTF8, 0, ucs2, length, nullptr, 0,
                                   nullptr, nullptr);
  if (size == 0)
    return result;
  result.resize(size);
  ::WideCharToMultiByte(CP_UTF8, 0, ucs2, length, result.data(), size, nullptr,
                        nullptr);
  return result;
}

} // namespace encoding
