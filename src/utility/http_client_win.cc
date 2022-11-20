#include <Windows.h>
#include <Winhttp.h>

#include "encoding.h"
#include "http_client.h"
#include "url.h"
#include <charconv>
#include <memory>
#include <sstream>
#include <string_view>

namespace {

class HttpSession : public std::enable_shared_from_this<HttpSession> {
public:
  HttpSession(HANDLE event, std::string user_agent, std::string url,
              std::string method, HttpClient::HttpHeader header,
              std::string body, HttpClient::HttpResponseHandler on_response,
              HttpClient::HttpErrorNotifier on_error, unsigned timeout)
      : event_(event), method_(std::move(method)),
        request_header_(std::move(header)), request_body_(std::move(body)),
        on_response_(std::move(on_response)), on_error_(std::move(on_error)),
        timeout_(timeout) {
    url_parts_.parse(std::move(url));
    std::wstring user_agent_w = encoding::UTF8ToUCS2(user_agent);
    session_ = ::WinHttpOpen(
        user_agent_w.c_str(), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC);
    ::WinHttpSetStatusCallback(session_,
                               HttpSession::StaticWinHttpStatusCallback,
                               WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS, 0);
  }
  ~HttpSession() { ::WinHttpCloseHandle(session_); }

  bool Start() {
    if (!Connect())
      return false;
    if (!SendRequest())
      return false;
    return true;
  }

private:
  bool Connect() {
    std::wstring host = encoding::UTF8ToUCS2(url_parts_.domain);
    INTERNET_PORT port = INTERNET_DEFAULT_PORT;
    if (url_parts_.port.length() > 0) {
      std::from_chars(url_parts_.port.data(),
                      url_parts_.port.data() + url_parts_.port.length(), port);
    }
    connection_ = ::WinHttpConnect(session_, host.c_str(), port, 0);
    if (connection_ == nullptr) {
      return false;
    }
    return true;
  }

  bool SendRequest() {
    std::wstring verb = encoding::UTF8ToUCS2(method_);
    std::wstring path = encoding::UTF8ToUCS2(
        !url_parts_.full_path.empty() ? url_parts_.full_path : "/");

    request_ = ::WinHttpOpenRequest(
        connection_, verb.c_str(), path.c_str(), NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_REFRESH);

    if (request_ == nullptr) {
      return false;
    }

    std::stringstream ss;
    for (const auto &h : request_header_) {
      ss << h.first << ": " << h.second << "\r\n";
    }
    std::wstring header_string_w = encoding::UTF8ToUCS2(ss.str());

    if (!::WinHttpSendRequest(
            request_, header_string_w.c_str(), header_string_w.length(),
            static_cast<LPVOID>(request_body_.data()), request_body_.length(),
            request_body_.length(), (DWORD_PTR)this)) {
      return false;
    }

    keep_this_ = shared_from_this();

    return true;
  }

  bool OnSendRequestComplete() {
    if (!::WinHttpReceiveResponse(request_, nullptr)) {
      return false;
    }
    return true;
  }

  bool OnHeadersAvailable() {
    std::wstring status_code;
    status_code.resize(3);
    DWORD dwStatusCodeSize = (status_code.size() + 1) * sizeof(wchar_t);
    ::WinHttpQueryHeaders(request_, WINHTTP_QUERY_STATUS_CODE,
                          WINHTTP_HEADER_NAME_BY_INDEX, status_code.data(),
                          &dwStatusCodeSize, WINHTTP_NO_HEADER_INDEX);
    status_code_ = _wtoi(status_code.c_str());

    DWORD dwHeaderSize = 0;
    ::WinHttpQueryHeaders(
        request_, WINHTTP_QUERY_RAW_HEADERS, WINHTTP_HEADER_NAME_BY_INDEX,
        WINHTTP_NO_OUTPUT_BUFFER, &dwHeaderSize, WINHTTP_NO_HEADER_INDEX);

    std::wstring buffer;
    buffer.resize(dwHeaderSize - 1);
    if (!::WinHttpQueryHeaders(request_, WINHTTP_QUERY_RAW_HEADERS,
                               WINHTTP_HEADER_NAME_BY_INDEX, buffer.data(),
                               &dwHeaderSize, WINHTTP_NO_HEADER_INDEX)) {
      return false;
    }
    for (size_t i = 0; i < buffer.length() && buffer[i] != '\0';) {
      std::wstring_view sv(buffer.c_str() + i);
      std::string line = encoding::UCS2ToUTF8(sv);
      char *p = strchr(line.data(), ':');
      if (p != nullptr) {
        *p++ = '\0';
        if (*p == ' ')
          ++p;
        response_header_.insert({line.c_str(), p});
      }
      i += line.length() + 1;
    }

    if (!::WinHttpQueryDataAvailable(request_, nullptr)) {
      return false;
    }
    return true;
  }

  bool OnDateAvailable(DWORD dwBytesAvailable) {
    buffer_.resize(dwBytesAvailable);
    if (!::WinHttpReadData(request_, buffer_.data(), dwBytesAvailable,
                           nullptr)) {
      return false;
    }
    return true;
  }

  bool OnReadComplete(LPVOID lpBuffer, DWORD dwTotalBytesRead) {
    if (dwTotalBytesRead > 0) {
      response_body_.append(static_cast<const char *>(lpBuffer),
                            dwTotalBytesRead);
      if (!::WinHttpQueryDataAvailable(request_, nullptr)) {
        return false;
      }
    } else {
      Callback();
    }
    return true;
  }

  void OnRequestError(LPWINHTTP_ASYNC_RESULT result) {
    if (on_error_) {
      const char *error = nullptr;
      switch (result->dwResult) {
      case API_RECEIVE_RESPONSE:
        error = "API_RECEIVE_RESPONSE";
        break;
      case API_QUERY_DATA_AVAILABLE:
        error = "API_QUERY_DATA_AVAILABLE";
        break;
      case API_READ_DATA:
        error = "API_READ_DATA";
        break;
      case API_WRITE_DATA:
        error = "API_WRITE_DATA";
        break;
      case API_SEND_REQUEST:
        error = "API_RECEIVE_RESPONSE";
        break;
      case API_GET_PROXY_FOR_URL:
        error = "API_RECEIVE_RESPONSE";
        break;
      default:
        break;
      }
      on_error_(error);
    }
    ::SetEvent(event_);
    keep_this_.reset();
  }

  void Callback() {
    if (on_response_) {
      HttpClient::HttpHeader header;
      for (auto &h : response_header_) {
        header.insert({h.first, h.second});
      }
      on_response_(status_code_, std::move(header), response_body_.c_str());
    }
    ::SetEvent(event_);
    keep_this_.reset();
  }

private:
  void CALLBACK WinHttpStatusCallback(HINTERNET hInternet,
                                      DWORD dwInternetStatus,
                                      LPVOID lpvStatusInformation,
                                      DWORD dwStatusInformationLength) {
    switch (dwInternetStatus) {
    case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
      OnSendRequestComplete();
      break;
    case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
      OnHeadersAvailable();
      break;
    case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
      OnDateAvailable(*(DWORD *)lpvStatusInformation);
      break;
    case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
      OnReadComplete(lpvStatusInformation, dwStatusInformationLength);
      break;
    case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
      OnRequestError((LPWINHTTP_ASYNC_RESULT)lpvStatusInformation);
      break;
    default:
      break;
    }
  }

private:
  static void CALLBACK StaticWinHttpStatusCallback(
      HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus,
      LPVOID lpvStatusInformation, DWORD dwStatusInformationLength) {
    ((HttpSession *)dwContext)
        ->WinHttpStatusCallback(hInternet, dwInternetStatus,
                                lpvStatusInformation,
                                dwStatusInformationLength);
  }

private:
  unsigned timeout_ = 0;
  HttpClient::HttpResponseHandler on_response_;
  HttpClient::HttpErrorNotifier on_error_;

  Url url_parts_;
  std::string method_;
  HttpClient::HttpHeader request_header_;
  std::string request_body_;
  HINTERNET session_;
  HANDLE event_ = nullptr;
  HINTERNET connection_;
  HINTERNET request_;
  std::shared_ptr<HttpSession> keep_this_;
  unsigned status_code_;
  HttpClient::HttpHeader response_header_;
  std::string buffer_;
  std::string response_body_;
};

bool start_session(HANDLE event, std::string user_agent, std::string url,
                   std::string method, HttpClient::HttpHeader header,
                   std::string body,
                   HttpClient::HttpResponseHandler on_response,
                   HttpClient::HttpErrorNotifier on_error, unsigned timeout) {
  auto session = std::make_shared<HttpSession>(
      event, user_agent, std::move(url), method, std::move(header), "",
      std::move(on_response), std::move(on_error), timeout);
  return session->Start();
}

} // namespace

HttpClient::HttpClient(std::string user_agent /*= ""*/)
    : user_agent_(std::move(user_agent)), waitable_context_() {
  waitable_context_ = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

HttpClient::~HttpClient() { ::CloseHandle((HANDLE)waitable_context_); }

bool HttpClient::Get(std::string url, HttpHeader header,
                     HttpResponseHandler on_response,
                     HttpErrorNotifier on_error, unsigned timeout /*= 0*/) {
  return start_session((HANDLE)waitable_context_, user_agent_, url, "GET",
                       std::move(header), "", std::move(on_response),
                       std::move(on_error), timeout);
}

bool HttpClient::Post(std::string url, HttpHeader header, std::string body,
                      HttpResponseHandler on_response,
                      HttpErrorNotifier on_error, unsigned timeout /*= 0*/) {
  return start_session((HANDLE)waitable_context_, user_agent_, url, "POST",
                       std::move(header), std::move(body),
                       std::move(on_response), std::move(on_error), timeout);
}

bool HttpClient::Put(std::string url, HttpHeader header, std::string body,
                     HttpResponseHandler on_response,
                     HttpErrorNotifier on_error, unsigned timeout /*= 0*/) {
  return start_session((HANDLE)waitable_context_, user_agent_, url, "PUT",
                       std::move(header), std::move(body),
                       std::move(on_response), std::move(on_error), timeout);
}

bool HttpClient::Delete(std::string url, HttpHeader header,
                        HttpResponseHandler on_response,
                        HttpErrorNotifier on_error, unsigned timeout /*= 0*/) {
  return start_session((HANDLE)waitable_context_, user_agent_, url, "DELETE",
                       std::move(header), "", std::move(on_response),
                       std::move(on_error), timeout);
}

void HttpClient::Wait() {
  ::WaitForSingleObject((HANDLE)waitable_context_, INFINITE);
}
