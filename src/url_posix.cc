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

#include <curl/curl.h>
#include <regex>
#include <xl/scope_exit>
#include <xl/url>

namespace xl {

namespace url {

std::string encode(const std::string &s) {
  CURL *curl = curl_easy_init();
  if (curl == nullptr) {
    return "";
  }
  XL_ON_BLOCK_EXIT(curl_easy_cleanup, curl);
  char *encoded = curl_easy_escape(curl, s.c_str(), s.length());
  if (encoded == nullptr) {
    return "";
  }
  XL_ON_BLOCK_EXIT(curl_free, encoded);
  return std::string(encoded);
}

std::string decode(const std::string &s) {
  CURL *curl = curl_easy_init();
  if (curl == nullptr) {
    return "";
  }
  XL_ON_BLOCK_EXIT(curl_easy_cleanup, curl);
  int decoded_len;
  char *decoded = curl_easy_unescape(curl, s.c_str(), s.length(), &decoded_len);
  if (decoded == nullptr) {
    return "";
  }
  XL_ON_BLOCK_EXIT(curl_free, decoded);
  return std::string(decoded, decoded_len);
}

} // namespace url

} // namespace xl
