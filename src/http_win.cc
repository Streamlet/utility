#include <memory>
#include <sstream>
#include <utility>
#include <xl/encoding>
#include <xl/http>
#include <xl/scope_exit>

#define NOMINMAX
#include <Windows.h>
#include <Winhttp.h>

namespace xl {

namespace http {

extern const char *DEFAULT_USER_AGENT;

void ParseHeader(const std::string &raw_header, Header &parsed_header);

namespace {

const wchar_t *PROTOCOL_HTTPS = L"https";
const wchar_t *PROTOCOL_HTTP = L"http";

int ParseUrl(const std::string url, bool &ssl, std::wstring &host, unsigned short &port, std::wstring &path) {
  std::wstring url_w = xl::encoding::utf8_to_utf16(url);

  URL_COMPONENTS urlComp = {sizeof(urlComp)};
  urlComp.dwSchemeLength = (DWORD)-1;
  urlComp.dwHostNameLength = (DWORD)-1;
  urlComp.dwUrlPathLength = (DWORD)-1;
  urlComp.dwExtraInfoLength = (DWORD)-1;
  if (!::WinHttpCrackUrl(url_w.c_str(), (DWORD)url.length(), 0, &urlComp)) {
    return ::GetLastError();
  }
  wstring_ref url_protocol(urlComp.lpszScheme, urlComp.dwSchemeLength);
  host.assign(urlComp.lpszHostName, urlComp.dwHostNameLength);
  path.clear();
  path.append(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);
  path.append(urlComp.lpszExtraInfo, urlComp.dwExtraInfoLength);

  ssl = urlComp.nScheme == INTERNET_SCHEME_HTTPS;
  port = urlComp.nPort;
  return ERROR_SUCCESS;
}

int SendHeader(HINTERNET hRequest, const Header &header, DataReader body_reader) {
  std::stringstream ss;
  for (const auto &h : header) {
    ss << h.first << ": " << h.second << "\r\n";
  }
  std::wstring header_string = xl::encoding::utf8_to_utf16(ss.str());
  long long body_size = 0;
  if (body_reader) {
    body_reader(nullptr, 0, &body_size);
  }
  if (!::WinHttpSendRequest(hRequest, header_string.c_str(), (DWORD)header_string.length(), nullptr, 0,
                            (DWORD)body_size, 0)) {
    return ::GetLastError();
  }
  return ERROR_SUCCESS;
}

int SendBody(HINTERNET hRequest, DataReader body_reader) {
  const DWORD BUFFER_SIZE = 1024;
  char *BUFFER[BUFFER_SIZE];
  while (true) {
    size_t bytes_to_write = body_reader(BUFFER, BUFFER_SIZE, nullptr);
    if (bytes_to_write == 0) {
      break;
    }
    DWORD bytes_written = 0;
    if (!::WinHttpWriteData(hRequest, BUFFER, (DWORD)bytes_to_write, &bytes_written)) {
      return ::GetLastError();
    }
    if (bytes_written != bytes_to_write) {
      return ERROR_WINHTTP_INTERNAL_ERROR;
    }
  }
  return ERROR_SUCCESS;
}

int ReceiveHeader(HINTERNET hRequest, StatusCode *status, Header *header) {
  if (status != nullptr) {
    DWORD status_code_size = 0;
    if (!::WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE, WINHTTP_HEADER_NAME_BY_INDEX,
                               WINHTTP_NO_OUTPUT_BUFFER, &status_code_size, WINHTTP_NO_HEADER_INDEX)) {
      DWORD dwError = ::GetLastError();
      if (dwError != ERROR_INSUFFICIENT_BUFFER) {
        return dwError;
      }
    }
    std::wstring status_code;
    status_code.resize(status_code_size * sizeof(wchar_t));
    if (!::WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE, WINHTTP_HEADER_NAME_BY_INDEX,
                               (void *)status_code.data(), &status_code_size, WINHTTP_NO_HEADER_INDEX)) {
      return ::GetLastError();
    }
    *status = (StatusCode)_wtoi(status_code.c_str());
  }

  if (header != nullptr) {
    DWORD header_size = 0;
    if (!::WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX,
                               WINHTTP_NO_OUTPUT_BUFFER, &header_size, WINHTTP_NO_HEADER_INDEX)) {
      DWORD error = ::GetLastError();
      if (error != ERROR_INSUFFICIENT_BUFFER) {
        return error;
      }
    }
    std::wstring header_buffer;
    header_buffer.resize(header_size);
    if (!::WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX,
                               (void *)header_buffer.data(), &header_size, WINHTTP_NO_HEADER_INDEX)) {
      return ::GetLastError();
    }
    std::string raw_header = xl::encoding::utf16_to_utf8(header_buffer);
    ParseHeader(raw_header, *header);
  }

  return ERROR_SUCCESS;
}

int ReceiveBody(HINTERNET hRequest, DataWriter body_writer) {
  const DWORD BUFFER_SIZE = 1024;
  char *BUFFER[BUFFER_SIZE];
  while (true) {
    DWORD bytes_available = 0;
    if (!::WinHttpQueryDataAvailable(hRequest, &bytes_available)) {
      return ::GetLastError();
    }
    if (bytes_available == 0) {
      break;
    }
    while (bytes_available > 0) {
      DWORD bytes_to_read = std::min(bytes_available, BUFFER_SIZE);
      DWORD bytes_read = 0;
      if (!::WinHttpReadData(hRequest, BUFFER, bytes_to_read, &bytes_read)) {
        return ::GetLastError();
      }
      if (bytes_read > 0) {
        if (body_writer(BUFFER, bytes_read) != bytes_read) {
          return ERROR_WINHTTP_INTERNAL_ERROR;
        }
      }
      bytes_available -= bytes_read;
    }
  }
  if (body_writer(nullptr, 0) != 0) {
    return ERROR_WINHTTP_INTERNAL_ERROR;
  }
  return ERROR_SUCCESS;
}

} // namespace

int send(const Request &request, Response *response, const Option *option) {
  bool ssl = false;
  std::wstring host, path;
  unsigned short port = 0;
  int error = ParseUrl(request.url, ssl, host, port, path);
  if (error != ERROR_SUCCESS) {
    return -error;
  }

  const char *user_agent = nullptr;
  if (option != nullptr && !option->user_agent.empty()) {
    user_agent = option->user_agent.c_str();
  } else {
    user_agent = DEFAULT_USER_AGENT;
  }
  std::wstring ua = encoding::utf8_to_utf16(user_agent);

  HINTERNET hSession =
      ::WinHttpOpen(ua.c_str(), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
  XL_ON_BLOCK_EXIT(WinHttpCloseHandle, hSession);

  DWORD options = WINHTTP_OPTION_REDIRECT_POLICY_DISALLOW_HTTPS_TO_HTTP;
  ::WinHttpSetOption(hSession, WINHTTP_OPTION_REDIRECT_POLICY, &options, sizeof(options));

  if (option != nullptr && option->timeout > 0) {
    ::WinHttpSetTimeouts(hSession, option->timeout, option->timeout, option->timeout, option->timeout);
  }

  HINTERNET hConnection = ::WinHttpConnect(hSession, host.c_str(), port, 0);
  if (hConnection == nullptr) {
    return -(int)::GetLastError();
  }
  XL_ON_BLOCK_EXIT(::WinHttpCloseHandle, hConnection);

  HINTERNET hRequest = ::WinHttpOpenRequest(
      hConnection, METHOD_NAME_W[request.method], path.empty() ? L"/" : path.c_str(), nullptr, WINHTTP_NO_REFERER,
      WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_REFRESH | (ssl ? WINHTTP_FLAG_SECURE : 0));

  if (hRequest == nullptr) {
    return -(int)::GetLastError();
  }
  XL_ON_BLOCK_EXIT(::WinHttpCloseHandle, hRequest);

  error = SendHeader(hRequest, request.header, request.body);
  if (error != ERROR_SUCCESS) {
    return -error;
  }
  if (request.body) {
    error = SendBody(hRequest, request.body);
    if (error != ERROR_SUCCESS) {
      return -error;
    }
  }

  if (!::WinHttpReceiveResponse(hRequest, nullptr)) {
    return -(int)::GetLastError();
  }

  StatusCode status;
  error = ReceiveHeader(hRequest, &status, nullptr);
  if (error != ERROR_SUCCESS) {
    return -error;
  }

  if (response != nullptr && response->header != nullptr) {
    error = ReceiveHeader(hRequest, nullptr, response->header);
    if (error != ERROR_SUCCESS) {
      return -error;
    }
  }

  if (response != nullptr && response->body != nullptr) {
    error = ReceiveBody(hRequest, response->body);
    if (error != ERROR_SUCCESS) {
      return -error;
    }
  }

  return status;
}

} // namespace http

} // namespace xl