// MIT License
//
// Copyright (c) 2022 Streamlet (streamlet@outlook.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <regex>
#include <xl/url>

#define NOMINMAX
#include <Windows.h>
#include <Winhttp.h>

namespace xl {

namespace url {

//
// The UrlEscape in ShellWAPI does not implement RFC 3986 (URL_ESCAPE_ASCII_URI_COMPONENT can be used since Win8).
// So here we write it
//

namespace {

bool is_rfc_3986_allowed_char(char ch) {
  if (ch >= '0' && ch <= '9') {
    return true;
  }
  if (ch >= 'A' && ch <= 'Z') {
    return true;
  }
  if (ch >= 'a' && ch <= 'z') {
    return true;
  }
  switch (ch) {
  case '-':
  case '.':
  case '_':
  case '~':
    return true;
  }
  return false;
}

bool is_hex_char(char ch) {
  if (ch >= '0' && ch <= '9') {
    return true;
  }
  if (ch >= 'A' && ch <= 'F') {
    return true;
  }
  if (ch >= 'a' && ch <= 'f') {
    return true;
  }
  return false;
}

char to_hex(char dec) {
  if (dec < 10) {
    return '0' + dec;
  } else {
    return 'A' + dec - 10;
  }
}

char to_dec(char ch) {
  if (ch >= '0' && ch <= '9') {
    return ch - '0';
  }
  if (ch >= 'A' && ch <= 'F') {
    return ch - 'A' + 10;
  }
  if (ch >= 'a' && ch <= 'f') {
    return ch - 'a' + 10;
  }
  return -1;
}

} // namespace

std::string encode(const std::string &s) {
  int convert_chars = 0;
  for (char ch : s) {
    if (!is_rfc_3986_allowed_char(ch)) {
      ++convert_chars;
    }
  }
  std::string r;
  r.reserve(s.length() + convert_chars * 2);
  for (char ch : s) {
    if (is_rfc_3986_allowed_char(ch)) {
      r.push_back(ch);
    } else {
      r.push_back('%');
      r.push_back(to_hex((unsigned char)ch / 0x10));
      r.push_back(to_hex((unsigned char)ch % 0x10));
    }
  }
  return r;
}

std::string decode(const std::string &s) {
  std::string r;
  r.reserve(s.length());
  for (size_t i = 0; i < s.length();) {
    if (s[i] == '%' && i + 2 < s.length() && is_hex_char(s[i + 1]) && is_hex_char(s[i + 2])) {
      r.push_back(to_dec(s[i + 1]) * 0x10 + to_dec(s[i + 2]));
      i += 3;
    } else {
      r.push_back(s[i]);
      ++i;
    }
  }
  return r;
}

} // namespace url

} // namespace xl
