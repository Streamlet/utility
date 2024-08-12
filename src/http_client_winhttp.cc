#include <Windows.h>
#include <Winhttp.h>

#include <charconv>
#include <memory>
#include <sstream>
#include <string_view>
#include <xl/encoding>
#include <xl/http_client>
#include <xl/scope_exit>
#include <xl/url>

namespace {

const wchar_t *PROTOCOL_HTTPS = L"https";
const wchar_t *PROTOCOL_HTTP = L"http";

class winhttp_error_category : public std::error_category {
  const char *name() const noexcept override {
    return "winhttp_error";
  }

  std::string message(int _Errval) const override {
    LPSTR buffer = nullptr;
    ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                     nullptr, _Errval, 0, reinterpret_cast<LPSTR>(&buffer), 0, nullptr);
    XL_ON_BLOCK_EXIT(::LocalFree, buffer);
    return buffer == nullptr ? "unknown error" : buffer;
  }
};

std::error_category &winhttp_category() {
  static winhttp_error_category instance;
  return instance;
}

std::error_code make_winhttp_error(DWORD error) {
  return std::error_code(error, winhttp_category());
}

} // namespace

void ParseHeader(const std::string &raw_header, HttpClient::ResponseHeader &parsed_header);
HttpClient::ResponseBodyReceiver StringBodyReceiver(std::string *response_body);

class HttpClient::HttpSession {
public:
  HttpSession(std::string user_agent) {
    std::wstring ua = xl::encoding::utf8_to_utf16(user_agent);

    session_ = ::WinHttpOpen(ua.empty() ? L"WinHttp" : ua.c_str(), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                             WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    DWORD options = WINHTTP_OPTION_REDIRECT_POLICY_DISALLOW_HTTPS_TO_HTTP;
    ::WinHttpSetOption(session_, WINHTTP_OPTION_REDIRECT_POLICY, &options, sizeof(options));
  }
  ~HttpSession() {
    ::WinHttpCloseHandle(session_);
  }

  std::error_code SendAndReceive(const wchar_t *method,
                                 const std::string_view &url_string,
                                 const RequestHeader &request_header,
                                 const std::string_view &request_body,
                                 unsigned *response_status,
                                 ResponseHeader *response_header,
                                 ResponseBodyReceiver response_body_receiver,
                                 unsigned timeout = 0) {
    if (timeout > 0) {
      ::WinHttpSetTimeouts(session_, timeout, timeout, timeout, timeout);
    }

    std::wstring url = xl::encoding::utf8_to_utf16(url_string.data(), url_string.length());
    URL_COMPONENTS urlComp = {sizeof(urlComp)};
    urlComp.dwSchemeLength = (DWORD)-1;
    urlComp.dwHostNameLength = (DWORD)-1;
    urlComp.dwUrlPathLength = (DWORD)-1;
    urlComp.dwExtraInfoLength = (DWORD)-1;
    if (!::WinHttpCrackUrl(url.c_str(), (DWORD)url.length(), 0, &urlComp)) {
      return std::make_error_code(std::errc::invalid_argument);
    }
    std::wstring_view url_protocol(urlComp.lpszScheme, urlComp.dwSchemeLength);
    std::wstring url_host(urlComp.lpszHostName, urlComp.dwHostNameLength);
    std::wstring url_path(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);
    std::wstring url_query(urlComp.lpszExtraInfo, urlComp.dwExtraInfoLength);

    if (url_protocol != PROTOCOL_HTTPS && url_protocol != PROTOCOL_HTTP) {
      return std::make_error_code(std::errc::protocol_not_supported);
    }
    bool ssl = url_protocol == PROTOCOL_HTTPS;

    HINTERNET connection = Connect(session_, url_host, urlComp.nPort);
    if (connection == nullptr) {
      return make_winhttp_error(::GetLastError());
    }
    XL_ON_BLOCK_EXIT(::WinHttpCloseHandle, connection);

    HINTERNET request = OpenRequest(connection, ssl, method, url_path + url_query);
    if (request == nullptr) {
      return make_winhttp_error(::GetLastError());
    }
    XL_ON_BLOCK_EXIT(::WinHttpCloseHandle, request);

    std::error_code ec = SendRequest(request, request_header, request_body);
    if (ec) {
      return ec;
    }

    if (response_status != nullptr || response_header != nullptr) {
      ec = ReceiveHeader(request, response_status, response_header);
      if (ec) {
        return ec;
      }
    }

    if (response_body_receiver != nullptr) {
      ec = ReceiveBody(request, response_body_receiver);
      if (ec) {
        return ec;
      }
    }

    return {};
  }

private:
  HINTERNET Connect(HINTERNET session, const std::wstring &host, INTERNET_PORT port) {
    return ::WinHttpConnect(session, host.c_str(), port, 0);
  }

  HINTERNET OpenRequest(HINTERNET connection, bool ssl, const wchar_t *method, const std::wstring &path) {
    return ::WinHttpOpenRequest(connection, method, path.empty() ? L"/" : path.c_str(), nullptr, WINHTTP_NO_REFERER,
                                WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_REFRESH | (ssl ? WINHTTP_FLAG_SECURE : 0));
  }

  std::error_code
  SendRequest(HINTERNET request, const RequestHeader &request_header, const std::string_view &request_body) {
    std::stringstream ss;
    for (const auto &h : request_header) {
      ss << h.first << ": " << h.second << "\r\n";
    }
    std::wstring header_string = xl::encoding::utf8_to_utf16(ss.str());
    if (!::WinHttpSendRequest(request, header_string.c_str(), (DWORD)header_string.length(),
                              const_cast<char *>(request_body.data()), (DWORD)request_body.length(),
                              (DWORD)request_body.length(), 0)) {
      return make_winhttp_error(::GetLastError());
    }
    if (!::WinHttpReceiveResponse(request, nullptr)) {
      return make_winhttp_error(::GetLastError());
    }

    return {};
  }

  std::error_code ReceiveHeader(HINTERNET request, unsigned *response_status, ResponseHeader *response_header) {
    if (response_status != nullptr) {
      DWORD status_code_size = 0;
      if (!::WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE, WINHTTP_HEADER_NAME_BY_INDEX,
                                 WINHTTP_NO_OUTPUT_BUFFER, &status_code_size, WINHTTP_NO_HEADER_INDEX)) {
        DWORD error = ::GetLastError();
        if (error != ERROR_INSUFFICIENT_BUFFER) {
          return make_winhttp_error(error);
        }
      }
      std::wstring status_code;
      status_code.resize(status_code_size * sizeof(wchar_t));
      if (!::WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE, WINHTTP_HEADER_NAME_BY_INDEX, status_code.data(),
                                 &status_code_size, WINHTTP_NO_HEADER_INDEX)) {
        return make_winhttp_error(::GetLastError());
      }
      *response_status = _wtoi(status_code.c_str());
    }

    if (response_header != nullptr) {
      DWORD header_size = 0;
      if (!::WinHttpQueryHeaders(request, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX,
                                 WINHTTP_NO_OUTPUT_BUFFER, &header_size, WINHTTP_NO_HEADER_INDEX)) {
        DWORD error = ::GetLastError();
        if (error != ERROR_INSUFFICIENT_BUFFER) {
          return make_winhttp_error(error);
        }
      }
      std::wstring buffer_w;
      buffer_w.resize(header_size);
      if (!::WinHttpQueryHeaders(request, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, buffer_w.data(),
                                 &header_size, WINHTTP_NO_HEADER_INDEX)) {
        return make_winhttp_error(::GetLastError());
      }
      std::string raw_header = xl::encoding::utf16_to_utf8(buffer_w);
      ParseHeader(raw_header, *response_header);
    }

    return {};
  }

  std::error_code ReceiveBody(HINTERNET request, ResponseBodyReceiver response_body_receiver) {
    DWORD bytes_available = 0;
    std::string buffer;
    while (true) {
      if (!::WinHttpQueryDataAvailable(request, &bytes_available)) {
        return make_winhttp_error(::GetLastError());
      }
      if (bytes_available == 0) {
        break;
      }
      while (bytes_available > 0) {
        buffer.resize(bytes_available);
        DWORD bytes_read = 0;
        if (!::WinHttpReadData(request, buffer.data(), bytes_available, &bytes_read)) {
          return make_winhttp_error(::GetLastError());
        }
        if (bytes_read > 0) {
          response_body_receiver(buffer.data(), bytes_read);
        }
        bytes_available -= bytes_read;
      }
    }
    return {};
  }

private:
  HINTERNET session_;
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
  return session_->SendAndReceive(L"HEAD", url, request_header, "", response_status, response_header, nullptr);
}

std::error_code HttpClient::Get(const std::string_view &url,
                                const RequestHeader &request_header,
                                unsigned *response_status,
                                ResponseHeader *response_header,
                                std::string *response_body,
                                unsigned timeout) {
  return session_->SendAndReceive(L"GET", url, request_header, "", response_status, response_header,
                                  StringBodyReceiver(response_body));
}

std::error_code HttpClient::Get(const std::string_view &url,
                                const RequestHeader &request_header,
                                unsigned *response_status,
                                ResponseHeader *response_header,
                                ResponseBodyReceiver response_body_receiver,
                                unsigned timeout) {
  return session_->SendAndReceive(L"GET", url, request_header, "", response_status, response_header,
                                  response_body_receiver);
}

std::error_code HttpClient::Post(const std::string_view &url,
                                 const RequestHeader &request_header,
                                 const std::string_view &request_body,
                                 unsigned *response_status,
                                 ResponseHeader *response_header,
                                 std::string *response_body,
                                 unsigned timeout) {
  return session_->SendAndReceive(L"POST", url, request_header, request_body, response_status, response_header,
                                  StringBodyReceiver(response_body));
}

std::error_code HttpClient::Post(const std::string_view &url,
                                 const RequestHeader &request_header,
                                 const std::string_view &request_body,
                                 unsigned *response_status,
                                 ResponseHeader *response_header,
                                 ResponseBodyReceiver response_body_receiver,
                                 unsigned timeout) {
  return session_->SendAndReceive(L"POST", url, request_header, request_body, response_status, response_header,
                                  response_body_receiver);
}

std::error_code HttpClient::Put(const std::string_view &url,
                                const RequestHeader &request_header,
                                const std::string_view &request_body,
                                unsigned *response_status,
                                ResponseHeader *response_header,
                                std::string *response_body,
                                unsigned timeout) {
  return session_->SendAndReceive(L"PUT", url, request_header, request_body, response_status, response_header,
                                  StringBodyReceiver(response_body));
}

std::error_code HttpClient::Delete(const std::string_view &url,
                                   const RequestHeader &request_header,
                                   unsigned *response_status,
                                   ResponseHeader *response_header,
                                   std::string *response_body,
                                   unsigned timeout) {
  return session_->SendAndReceive(L"DELETE", url, request_header, "", response_status, response_header,
                                  StringBodyReceiver(response_body));
}
