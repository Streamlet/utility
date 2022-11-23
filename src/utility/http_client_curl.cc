#include "http_client.h"
#include <boost/scope_exit.hpp>
#include <curl/curl.h>
#include <iostream>
#include <memory>

namespace {

const unsigned HTTP_VERSION                 = 11;
const char *PROTOCOL_HTTPS                  = "https";
const char *PROTOCOL_HTTP                   = "http";
const char *INTERNET_DEFAULT_HTTPS_PORT     = "443";
const char *INTERNET_DEFAULT_HTTP_PORT      = "80";
const unsigned CLOSE_CONNECTION_MAX_TIMEOUT = 1000;

enum class HttpMethod : int {
  Get = 0,
  Post,
  Put,
  Delete,
};

struct CUrlReadContext {
  const char *data = nullptr;
  size_t total     = 0;
  size_t read      = 0;
};

size_t read_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
  CUrlReadContext *ctx = static_cast<CUrlReadContext *>(userdata);

  size_t len = ctx->total - ctx->read;
  if (len > size * nitems)
    len = size * nitems;
  memcpy(buffer, ctx->data + ctx->read, len);
  ctx->data += len;
  return len;
}

struct CUrlWriteContext {
  std::string &receiver;
};

size_t write_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
  CUrlWriteContext *ctx = static_cast<CUrlWriteContext *>(userdata);
  ctx->receiver.append(buffer, buffer + size * nitems);
  return size * nitems;
}

} // namespace

void ParseHeader(const std::string &raw_header, HttpClient::ResponseHeader &parsed_header);

class HttpClient::HttpSession {
public:
  HttpSession(const std::string &user_agent) : user_agent_(std::move(user_agent)) {
  }
  ~HttpSession() {
  }
  std::error_code SendAndReceive(HttpMethod method,
                                 const std::string_view &url_string,
                                 const RequestHeader &request_header,
                                 const std::string_view &request_body,
                                 unsigned *response_status,
                                 ResponseHeader *response_header,
                                 std::string *response_body,
                                 unsigned timeout = 0) {
    CURL *curl = curl_easy_init();
    BOOST_SCOPE_EXIT(curl) {
      curl_easy_cleanup(curl);
    }
    BOOST_SCOPE_EXIT_END;

    std::string url(url_string);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

    switch (method) {
    case HttpMethod::Get:
      curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
      break;
    case HttpMethod::Post:
      curl_easy_setopt(curl, CURLOPT_POST, 1);
      break;
    case HttpMethod::Put:
      curl_easy_setopt(curl, CURLOPT_PUT, 1);
      break;
    case HttpMethod::Delete:
    default:
      return std::make_error_code(std::errc::operation_not_supported);
    }

    curl_slist *header = nullptr;
    if (!request_header.empty()) {
      for (const auto &h : request_header) {
        std::string header_line;
        header_line += h.first;
        header_line += ": ";
        header_line += h.second;
        header = curl_slist_append(header, header_line.c_str());
      }
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    }
    BOOST_SCOPE_EXIT(&request_header, header) {
      if (!request_header.empty())
        curl_slist_free_all(header);
    }
    BOOST_SCOPE_EXIT_END;

    CUrlReadContext read_ctx{request_body.data(), request_body.size(), 0};
    if (!request_body.empty()) {
      curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
      curl_easy_setopt(curl, CURLOPT_READDATA, &read_ctx);
    }

    std::string raw_header;
    CUrlWriteContext header_ctx{raw_header};
    if (response_header != nullptr) {
      curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_callback);
      curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_ctx);
    }
    CUrlWriteContext write_ctx{*response_body};
    if (response_body != nullptr) {
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &write_ctx);
    }

    CURLcode code = curl_easy_perform(curl);
    if (code != CURLE_OK) {
      return std::error_code(code, std::generic_category());
    }

    if (response_status != nullptr) {
      code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, response_status);
      if (code != CURLE_OK) {
        return std::error_code(code, std::generic_category());
      }
    }

    if (response_header != nullptr) {
      ParseHeader(raw_header, *response_header);
    }

    return {};
  }

private:
  std::string user_agent_;
};

HttpClient::HttpClient(std::string user_agent) : session_(std::make_unique<HttpSession>(std::move(user_agent))) {
}

HttpClient::~HttpClient() {
}

std::error_code HttpClient::Get(const std::string_view &url,
                                const RequestHeader &request_header,
                                unsigned *response_status,
                                ResponseHeader *response_header,
                                std::string *response_body,
                                unsigned timeout) {
  return session_->SendAndReceive(HttpMethod::Get, url, request_header, "", response_status, response_header,
                                  response_body);
}

std::error_code HttpClient::Post(const std::string_view &url,
                                 const RequestHeader &request_header,
                                 const std::string_view &request_body,
                                 unsigned *response_status,
                                 ResponseHeader *response_header,
                                 std::string *response_body,
                                 unsigned timeout) {
  return session_->SendAndReceive(HttpMethod::Post, url, request_header, request_body, response_status, response_header,
                                  response_body);
}

std::error_code HttpClient::Put(const std::string_view &url,
                                const RequestHeader &request_header,
                                const std::string_view &request_body,
                                unsigned *response_status,
                                ResponseHeader *response_header,
                                std::string *response_body,
                                unsigned timeout) {
  return session_->SendAndReceive(HttpMethod::Put, url, request_header, request_body, response_status, response_header,
                                  response_body);
}

std::error_code HttpClient::Delete(const std::string_view &url,
                                   const RequestHeader &request_header,
                                   unsigned *response_status,
                                   ResponseHeader *response_header,
                                   std::string *response_body,
                                   unsigned timeout) {
  return session_->SendAndReceive(HttpMethod::Delete, url, request_header, "", response_status, response_header,
                                  response_body);
}
