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
extern const wchar_t *METHOD_NAME_W[METHOD_COUNT];

void parse_header(const std::string &raw_headers, Headers &parsed_headers);

namespace {

const wchar_t *PROTOCOL_HTTPS = L"https";
const wchar_t *PROTOCOL_HTTP = L"http";

int parse_url(const std::string url, bool &ssl, std::wstring &host, unsigned short &port, std::wstring &path) {
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

int send_headers(HINTERNET hRequest, const Headers &headers, DataReader body_reader) {
  std::stringstream ss;
  for (const auto &h : headers) {
    ss << h.first << ": " << h.second << "\r\n";
  }
  std::wstring headers_string = xl::encoding::utf8_to_utf16(ss.str());
  long long body_size = 0;
  if (body_reader) {
    body_reader(nullptr, 0, &body_size);
  }
  if (!::WinHttpSendRequest(hRequest, headers_string.c_str(), (DWORD)headers_string.length(), nullptr, 0,
                            (DWORD)body_size, 0)) {
    return ::GetLastError();
  }
  return ERROR_SUCCESS;
}

int send_body(HINTERNET hRequest, DataReader body_reader) {
  const DWORD BUFFER_SIZE = 1024;
  char BUFFER[BUFFER_SIZE];
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

int receive_headers(HINTERNET hRequest, Status *status, Headers *headers) {
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
    *status = (Status)_wtoi(status_code.c_str());
  }

  if (headers != nullptr) {
    DWORD headers_size = 0;
    if (!::WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX,
                               WINHTTP_NO_OUTPUT_BUFFER, &headers_size, WINHTTP_NO_HEADER_INDEX)) {
      DWORD error = ::GetLastError();
      if (error != ERROR_INSUFFICIENT_BUFFER) {
        return error;
      }
    }
    std::wstring headers_buffer;
    headers_buffer.resize(headers_size);
    if (!::WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX,
                               (void *)headers_buffer.data(), &headers_size, WINHTTP_NO_HEADER_INDEX)) {
      return ::GetLastError();
    }
    std::string raw_headers = xl::encoding::utf16_to_utf8(headers_buffer);
    parse_header(raw_headers, *headers);
  }

  return ERROR_SUCCESS;
}

int receive_body(HINTERNET hRequest, DataWriter body_writer) {
  const DWORD BUFFER_SIZE = 1024;
  char BUFFER[BUFFER_SIZE];
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
  int error = parse_url(request.url, ssl, host, port, path);
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
  if (hSession == nullptr) {
    return -(int)::GetLastError();
  }
  XL_ON_BLOCK_EXIT(WinHttpCloseHandle, hSession);

  if (option != nullptr && option->timeout > 0) {
    if (!::WinHttpSetTimeouts(hSession, option->timeout, option->timeout, option->timeout, option->timeout)) {
      return -(int)::GetLastError();
    }
  }

  DWORD options = 0;
  if (option == nullptr || option->follow_redirect) {
    options = WINHTTP_OPTION_REDIRECT_POLICY_DISALLOW_HTTPS_TO_HTTP;
  } else {
    options = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
  }
  if (!::WinHttpSetOption(hSession, WINHTTP_OPTION_REDIRECT_POLICY, &options, sizeof(options))) {
    return -(int)::GetLastError();
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

  error = send_headers(hRequest, request.headers, request.body);
  if (error != ERROR_SUCCESS) {
    return -error;
  }
  if (request.body) {
    error = send_body(hRequest, request.body);
    if (error != ERROR_SUCCESS) {
      return -error;
    }
  }

  if (!::WinHttpReceiveResponse(hRequest, nullptr)) {
    return -(int)::GetLastError();
  }

  Status status;
  error = receive_headers(hRequest, &status, nullptr);
  if (error != ERROR_SUCCESS) {
    return -error;
  }

  if (response != nullptr && response->headers != nullptr) {
    error = receive_headers(hRequest, nullptr, response->headers);
    if (error != ERROR_SUCCESS) {
      return -error;
    }
  }

  if (response != nullptr && response->body != nullptr) {
    error = receive_body(hRequest, response->body);
    if (error != ERROR_SUCCESS) {
      return -error;
    }
  }

  return status;
}

} // namespace http

} // namespace xl
