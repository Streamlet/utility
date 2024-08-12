#include <cstring>
#include <xl/http_client>

namespace xl {

const char *HttpClient::DEFAULT_USER_AGENT =
    "Mozilla/5.0 (compatible; MSIE 7.0; Windows NT 10.0; Win64; x64; Trident/7.0) "
    "AppleWebKit/500.00 (KHTML, like Gecko) "
    "Chrome/100.0.0.0 "
    "Safari/500.00 "
    "Edg/100.0.0.0";

void ParseHeader(const std::string &raw_header, HttpClient::ResponseHeader &parsed_header) {
  for (size_t i = 0; i < raw_header.length();) {
    const char *p = raw_header.c_str() + i;
    const char *crlf = strstr(p, "\r\n");
    if (crlf == nullptr) {
      break;
    }
    const char *colon = strstr(p, ": ");
    if (colon != nullptr && colon <= crlf) {
      parsed_header.insert(std::make_pair(std::string(p, colon), std::string(colon + 2, crlf)));
    }
    i += crlf - p + 2;
  }
}

HttpClient::ResponseBodyReceiver StringBodyReceiver(std::string *response_body) {
  if (response_body == nullptr) {
    return nullptr;
  }
  return [response_body](const void *data, size_t length) {
    response_body->append(static_cast<const char *>(data), length);
  };
}

} // namespace xl
