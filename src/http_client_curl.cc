#include <cstring>
#include <curl/curl.h>
#include <memory>
#include <xl/http_client>
#include <xl/scope_exit>

namespace {

enum class HttpMethod : int {
  Head = 0,
  Get,
  Post,
  Put,
  Delete,
};

struct CUrlReadContext {
  const char *data = nullptr;
  size_t total = 0;
  size_t read = 0;
};

size_t read_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
  CUrlReadContext *ctx = static_cast<CUrlReadContext *>(userdata);

  size_t len = ctx->total - ctx->read;
  if (len > size * nitems) {
    len = size * nitems;
  }
  memcpy(buffer, ctx->data + ctx->read, len);
  ctx->data += len;
  return len;
}

struct CUrlWriteHeaderContext {
  std::string &receiver;
};

size_t write_header_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
  CUrlWriteHeaderContext *ctx = static_cast<CUrlWriteHeaderContext *>(userdata);
  ctx->receiver.append(buffer, buffer + size * nitems);
  return size * nitems;
}
struct CUrlWriteBodyContext {
  HttpClient::ResponseBodyReceiver response_body_receiver;
};

size_t write_body_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
  CUrlWriteBodyContext *ctx = static_cast<CUrlWriteBodyContext *>(userdata);
  ctx->response_body_receiver(buffer, size * nitems);
  return size * nitems;
}

class curl_error_category : public std::error_category {
  const char *name() const noexcept override {
    return "curl_error";
  }

  std::string message(int _Errval) const override {
    const char *error = curl_easy_strerror(static_cast<CURLcode>(_Errval));
    return error == nullptr ? "unknown error" : error;
  }
};

std::error_category &curl_category() {
  static curl_error_category instance;
  return instance;
}

std::error_code make_curl_error(CURLcode curl_code) {
  return std::error_code(curl_code, curl_category());
}

} // namespace

void ParseHeader(const std::string &raw_header, HttpClient::ResponseHeader &parsed_header);
HttpClient::ResponseBodyReceiver StringBodyReceiver(std::string *response_body);

class HttpClient::HttpSession {
public:
  HttpSession(std::string user_agent) : user_agent_(std::move(user_agent)) {
  }
  ~HttpSession() {
  }
  std::error_code SendAndReceive(HttpMethod method,
                                 const std::string_view &url_string,
                                 const RequestHeader &request_header,
                                 const std::string_view &request_body,
                                 unsigned *response_status,
                                 ResponseHeader *response_header,
                                 ResponseBodyReceiver response_body_receiver,
                                 unsigned timeout = 0) {
    CURL *curl = curl_easy_init();
    if (curl == nullptr) {
      return make_curl_error(CURLE_FAILED_INIT);
    }
    XL_ON_BLOCK_EXIT(curl_easy_cleanup, curl);

    CURLcode error = curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent_.empty() ? "cURL" : user_agent_.c_str());
    if (error != CURLE_OK) {
      return make_curl_error(error);
    }

    std::string url(url_string);
    error = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    if (error != CURLE_OK) {
      return make_curl_error(error);
    }

    error = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    if (error != CURLE_OK) {
      return make_curl_error(error);
    }

    switch (method) {
    case HttpMethod::Head:
      error = curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
      break;
    case HttpMethod::Get:
      error = curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
      break;
    case HttpMethod::Post:
      error = curl_easy_setopt(curl, CURLOPT_POST, 1);
      break;
    case HttpMethod::Put:
      error = curl_easy_setopt(curl, CURLOPT_PUT, 1);
      break;
    case HttpMethod::Delete:
      error = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
      break;
    default:
      return make_curl_error(CURLE_UNSUPPORTED_PROTOCOL);
    }
    if (error != CURLE_OK) {
      return make_curl_error(error);
    }

    curl_slist *header = nullptr;
    XL_ON_BLOCK_EXIT(curl_slist_free_all, header);
    if (!request_header.empty()) {
      for (const auto &h : request_header) {
        std::string header_line;
        header_line += h.first;
        header_line += ": ";
        header_line += h.second;
        header = curl_slist_append(header, header_line.c_str());
      }
      error = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
      if (error != CURLE_OK) {
        return make_curl_error(error);
      }
    }

    CUrlReadContext read_ctx{request_body.data(), request_body.size(), 0};
    if (!request_body.empty()) {
      error = curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
      if (error != CURLE_OK) {
        return make_curl_error(error);
      }
      error = curl_easy_setopt(curl, CURLOPT_READDATA, &read_ctx);
      if (error != CURLE_OK) {
        return make_curl_error(error);
      }
    }

    std::string raw_header;
    CUrlWriteHeaderContext header_ctx{raw_header};
    if (response_header != nullptr) {
      error = curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_header_callback);
      if (error != CURLE_OK) {
        return make_curl_error(error);
      }
      error = curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_ctx);
      if (error != CURLE_OK) {
        return make_curl_error(error);
      }
    }
    CUrlWriteBodyContext write_ctx{response_body_receiver};
    if (response_body_receiver != nullptr) {
      error = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_body_callback);
      if (error != CURLE_OK) {
        return make_curl_error(error);
      }
      error = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &write_ctx);
      if (error != CURLE_OK) {
        return make_curl_error(error);
      }
    }

    error = curl_easy_perform(curl);
    if (error != CURLE_OK) {
      return make_curl_error(error);
    }

    if (response_status != nullptr) {
      long response_code = 0;
      error = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
      if (error != CURLE_OK) {
        return make_curl_error(error);
      }
      *response_status = (unsigned)response_code;
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

std::error_code HttpClient::Head(const std::string_view &url,
                                 const RequestHeader &request_header,
                                 unsigned *response_status,
                                 ResponseHeader *response_header,
                                 unsigned timeout) {
  return session_->SendAndReceive(HttpMethod::Head, url, request_header, "", response_status, response_header, nullptr);
}

std::error_code HttpClient::Get(const std::string_view &url,
                                const RequestHeader &request_header,
                                unsigned *response_status,
                                ResponseHeader *response_header,
                                std::string *response_body,
                                unsigned timeout) {
  return session_->SendAndReceive(HttpMethod::Get, url, request_header, "", response_status, response_header,
                                  StringBodyReceiver(response_body));
}

std::error_code HttpClient::Get(const std::string_view &url,
                                const RequestHeader &request_header,
                                unsigned *response_status,
                                ResponseHeader *response_header,
                                ResponseBodyReceiver response_body_receiver,
                                unsigned timeout) {
  return session_->SendAndReceive(HttpMethod::Get, url, request_header, "", response_status, response_header,
                                  response_body_receiver);
}

std::error_code HttpClient::Post(const std::string_view &url,
                                 const RequestHeader &request_header,
                                 const std::string_view &request_body,
                                 unsigned *response_status,
                                 ResponseHeader *response_header,
                                 std::string *response_body,
                                 unsigned timeout) {
  return session_->SendAndReceive(HttpMethod::Post, url, request_header, request_body, response_status, response_header,
                                  StringBodyReceiver(response_body));
}

std::error_code HttpClient::Post(const std::string_view &url,
                                 const RequestHeader &request_header,
                                 const std::string_view &request_body,
                                 unsigned *response_status,
                                 ResponseHeader *response_header,
                                 ResponseBodyReceiver response_body_receiver,
                                 unsigned timeout) {
  return session_->SendAndReceive(HttpMethod::Post, url, request_header, request_body, response_status, response_header,
                                  response_body_receiver);
}

std::error_code HttpClient::Put(const std::string_view &url,
                                const RequestHeader &request_header,
                                const std::string_view &request_body,
                                unsigned *response_status,
                                ResponseHeader *response_header,
                                std::string *response_body,
                                unsigned timeout) {
  return session_->SendAndReceive(HttpMethod::Put, url, request_header, request_body, response_status, response_header,
                                  StringBodyReceiver(response_body));
}

std::error_code HttpClient::Delete(const std::string_view &url,
                                   const RequestHeader &request_header,
                                   unsigned *response_status,
                                   ResponseHeader *response_header,
                                   std::string *response_body,
                                   unsigned timeout) {
  return session_->SendAndReceive(HttpMethod::Delete, url, request_header, "", response_status, response_header,
                                  StringBodyReceiver(response_body));
}
