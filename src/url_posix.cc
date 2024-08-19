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
