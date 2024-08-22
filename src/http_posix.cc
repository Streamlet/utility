#include <cstring>
#include <curl/curl.h>
#include <memory>
#include <xl/http>
#include <xl/scope_exit>

namespace xl {

namespace http {

namespace {

size_t read_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
  DataReader *reader = static_cast<DataReader *>(userdata);
  return (*reader)(buffer, size * nitems, nullptr);
}

size_t write_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
  DataWriter *writer = static_cast<DataWriter *>(userdata);
  return (*writer)(buffer, size * nitems);
}

} // namespace

extern const char *DEFAULT_USER_AGENT;
extern const char *METHOD_NAME[METHOD_COUNT];

void parse_header(const std::string &raw_headers, Headers &parsed_headers);

int send(const Request &request, Response *response, const Option *option) {
  CURL *curl = curl_easy_init();
  if (curl == nullptr) {
    return -CURLE_FAILED_INIT;
  }
  XL_ON_BLOCK_EXIT(curl_easy_cleanup, curl);
  CURLcode error = CURLE_OK;

  if (option != nullptr && !option->user_agent.empty()) {
    error = curl_easy_setopt(curl, CURLOPT_USERAGENT, option->user_agent.c_str());
  } else {
    error = curl_easy_setopt(curl, CURLOPT_USERAGENT, DEFAULT_USER_AGENT);
  }
  if (error != CURLE_OK) {
    return -error;
  }

  if (option != nullptr && option->timeout != 0) {
    error = curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, option->timeout);
    if (error != CURLE_OK) {
      return -error;
    }
  }

  if (option == nullptr || option->follow_redirect) {
    error = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
  } else {
    error = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0);
  }
  if (error != CURLE_OK) {
    return -error;
  }

  switch (request.method) {
  case MEHOD_OPTIONS:
  case METHOD_HEAD:
    error = curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
    break;
  case METHOD_GET:
    error = curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
    break;
  case METHOD_POST:
    error = curl_easy_setopt(curl, CURLOPT_POST, 1);
    break;
  case METHOD_PUT:
    error = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
    break;
  case METHOD_DELETE:
  case METHOD_TRACE:
  case METHOD_CONNECT:
    error = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, METHOD_NAME[request.method]);
    break;
  default:
    return -CURLE_UNSUPPORTED_PROTOCOL;
  }
  if (error != CURLE_OK) {
    return -error;
  }

  error = curl_easy_setopt(curl, CURLOPT_URL, request.url.c_str());
  if (error != CURLE_OK) {
    return -error;
  }

  curl_slist *headers = nullptr;
  XL_ON_BLOCK_EXIT(curl_slist_free_all, headers);
  headers = curl_slist_append(headers, "Content-Type:");
  headers = curl_slist_append(headers, "Expect:");
  if (!request.headers.empty()) {
    for (const auto &h : request.headers) {
      std::string header_line;
      header_line += h.first;
      header_line += ": ";
      header_line += h.second;
      headers = curl_slist_append(headers, header_line.c_str());
    }
  }
  error = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  if (error != CURLE_OK) {
    return -error;
  }

  long long content_length = 0;
  if (request.body) {
    request.body(nullptr, 0, &content_length);
  }
  if (content_length > 0) {
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, NULL);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)content_length);
  }

  if (request.body) {
    error = curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    if (error != CURLE_OK) {
      return -error;
    }
    error = curl_easy_setopt(curl, CURLOPT_READDATA, &request.body);
    if (error != CURLE_OK) {
      return -error;
    }
  }
  std::string raw_headers;
  DataWriter headers_writer = buffer_writer(&raw_headers);
  if (response != nullptr && response->headers) {
    error = curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_callback);
    if (error != CURLE_OK) {
      return -error;
    }
    error = curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headers_writer);
    if (error != CURLE_OK) {
      return -error;
    }
  }

  if (response != nullptr && response->body) {
    error = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    if (error != CURLE_OK) {
      return -error;
    }
    error = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response->body);
    if (error != CURLE_OK) {
      return -error;
    }
  }

  error = curl_easy_perform(curl);
  if (error != CURLE_OK) {
    return -error;
  }

  long status;
  error = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
  if (error != CURLE_OK) {
    return -error;
  }

  if (response != nullptr && response->headers != nullptr) {
    parse_header(raw_headers, *response->headers);
  }

  return (int)status;
}

} // namespace http

} // namespace xl