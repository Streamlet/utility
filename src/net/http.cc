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

#include <cstring>
#include <random>
#include <sstream>
#include <utility>
#include <xl/encoding>
#include <xl/file>
#include <xl/http>
#include <xl/string>
#include <xl/url>

namespace xl {

namespace http {

const char *METHOD_NAME[METHOD_COUNT] = {
    "OPTIONS", "HEAD", "GET", "POST", "PUT", "DELETE", "TRACE", "CONNECT",
};

const wchar_t *METHOD_NAME_W[METHOD_COUNT] = {
    L"OPTIONS", L"HEAD", L"GET", L"POST", L"PUT", L"DELETE", L"TRACE", L"CONNECT",
};

const char *DEFAULT_USER_AGENT = "Mozilla/5.0 (compatible; MSIE 7.0; Windows NT 10.0; Win64; x64; Trident/7.0) "
                                 "AppleWebKit/500.00 (KHTML, like Gecko) "
                                 "Chrome/100.0.0.0 "
                                 "Safari/500.00 "
                                 "Edg/100.0.0.0";

DataReader buffer_reader(std::string string_buffer) {
  size_t bytes_read = 0;
  return [sb = std::move(string_buffer), bytes_read](void *buffer, size_t size, long long *total_size) mutable -> size_t {
    size_t bytes_copy = std::min(size, sb.length() - bytes_read);
    if (bytes_copy > 0) {
      memcpy(buffer, sb.c_str() + bytes_read, bytes_copy);
      bytes_read += bytes_copy;
    }
    if (total_size != nullptr) {
      *total_size = sb.length();
    }
    return bytes_copy;
  };
}

DataReader file_reader(const TCHAR *path) {
  long long file_size = fs::size(path);
  FILE *f = _tfopen(path, _T("rb"));
  return [file_size, f](void *buffer, size_t size, long long *total_size) -> size_t {
    if (f == NULL) {
      return -1;
    }
    if (feof(f)) {
      fclose(f);
      return 0;
    }
    size_t bytes_read = 0;
    if (size > 0) {
      bytes_read = fread(buffer, 1, size, f);
    }
    if (bytes_read != size) {
      fclose(f);
    }
    if (total_size != nullptr) {
      *total_size = file_size;
    }
    return bytes_read;
  };
}

DataWriter buffer_writer(std::string *string_buffer) {
  return [string_buffer](const void *buffer, size_t size) -> size_t {
    string_buffer->append((const char *)buffer, size);
    return size;
  };
}

DataWriter file_writer(const TCHAR *path) {
  FILE *f = _tfopen(path, _T("wb"));
  return [f](const void *buffer, size_t size) -> size_t {
    if (f == NULL) {
      return -1;
    }
    if (size == 0) {
      fclose(f);
      return 0;
    }
    size_t bytes_written = fwrite(buffer, 1, size, f);
    if (bytes_written != size) {
      fclose(f);
    }
    return bytes_written;
  };
}

std::string build_query_string(const FormData &form_data) {
  std::stringstream ss;
  for (const auto &item : form_data) {
    ss << url::encode(item.first) << '=' << url::encode(item.second) << '&';
  }
  std::string r = ss.str();
  if (!r.empty()) {
    r.resize(r.size() - 1);
  }
  return r;
}

FormData parse_query_string(const std::string &query_string) {
  FormData r;
  auto parts = xl::string::split_ref(query_string, "&");
  for (const auto &p : parts) {
    auto kv = xl::string::split(p.data(), "=", 2, p.length());
    if (!kv.empty()) {
      r.insert(std::make_pair(kv[0], kv.size() > 1 ? kv[1] : ""));
    }
  }
  return r;
}

void parse_header(const std::string &raw_headers, Headers &parsed_headers) {
  for (const char *p = raw_headers.c_str(); *p != '\0';) {
    while (*p == ' ' || *p == '\t') {
      ++p;
    }
    const char *crlf = strstr(p, "\r\n");
    if (crlf == nullptr) {
      break;
    }
    const char *colon = (const char *)memchr(p, ':', crlf - p);
    if (colon != nullptr && colon <= crlf) {
      const char *key_end = colon;
      while (key_end > p && (*(key_end - 1) == ' ' || *(key_end - 1) == '\t')) {
        --key_end;
      }
      const char *value = colon + 1;
      while (value < crlf && (*value == ' ' || *value == '\t')) {
        ++value;
      }
      const char *value_end = crlf;
      while (value_end > value && (*(value_end - 1) == ' ' || *(value_end - 1) == '\t')) {
        --value_end;
      }
      parsed_headers.emplace(std::string(p, key_end), std::string(value, value_end));
    }
    p = crlf + 2;
  }
}

std::string escape_quoted_value(const std::string &value) {
  return xl::string::replace(xl::string::replace(value, "\\", "\\\\"), "\"", "\\\"");
}

std::string generate_boundary() {
  static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz"
                                 "0123456789"
                                 "+/=";
  thread_local std::mt19937_64 rng(std::random_device{}());
  static std::uniform_int_distribution<> dist(0, sizeof(alphabet) - 2);
  std::stringstream ss;
  for (int i = 0; i < 30; ++i) {
    ss << alphabet[dist(rng)];
  }
  return ss.str();
}

std::string encode_multipart_form(const MultiPartFormData &form_data, std::string &boundary) {
  std::vector<std::string> flat_data;
  flat_data.reserve(form_data.size());
  for (const auto &item : form_data) {
    std::stringstream ss;
    ss << "Content-Disposition: form-data; name=\"" << escape_quoted_value(item.first) << "\"";
    if (!item.second.file_path.empty()) {
      std::string filename = xl::encoding::native_to_utf8(xl::path::filename(item.second.file_path.c_str()));
      ss << "; filename=\"" << escape_quoted_value(filename) << "\"";
    }
    ss << "\r\n\r\n";
    if (!item.second.file_path.empty()) {
      ss << xl::file::read(item.second.file_path.c_str());
    } else {
      ss << item.second.value;
    }
    ss << "\r\n";
    flat_data.push_back(ss.str());
  }
  std::string content = xl::string::join(flat_data, "\r\n");
  do {
    boundary = generate_boundary();
  } while (content.find(boundary) != std::string::npos);
  content = xl::string::join(flat_data, "--" + boundary + "\r\n");
  return "--" + boundary + "\r\n" + content + "--" + boundary + "--\r\n";
}

int get(const std::string &url, DataWriter response_body) {
  Request request;
  request.method = METHOD_GET;
  request.url = url;
  Response response;
  response.body = response_body;
  return send(request, &response, nullptr);
}

int get(const std::string &url, const Headers &request_headers, Headers &response_headers, DataWriter response_body) {
  Request request;
  request.method = METHOD_GET;
  request.url = url;
  request.headers = request_headers;
  Response response;
  response.headers = &response_headers;
  response.body = response_body;
  return send(request, &response, nullptr);
}

int post(const std::string &url, DataReader request_body, DataWriter response_body) {
  Request request;
  request.method = METHOD_POST;
  request.url = url;
  request.body = request_body;
  Response response;
  response.body = response_body;
  return send(request, &response, nullptr);
}

int post(const std::string &url,
         const Headers &request_headers,
         DataReader request_body,
         Headers &response_headers,
         DataWriter response_body) {
  Request request;
  request.method = METHOD_POST;
  request.url = url;
  request.headers = request_headers;
  request.body = request_body;
  Response response;
  response.headers = &response_headers;
  response.body = response_body;
  return send(request, &response, nullptr);
}

int post_form(const std::string &url,
              const Headers &request_headers,
              const FormData &form_data,
              Response *response,
              const Option *option) {
  Request request;
  request.method = METHOD_POST;
  request.url = url;
  request.headers = request_headers;
  request.headers.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
  request.body = buffer_reader(build_query_string(form_data));
  return send(request, response, option);
}

int post_form(const std::string &url, const FormData &form_data, DataWriter response_body) {
  Response response;
  response.body = response_body;
  return post_form(url, {}, form_data, &response);
}

int post_form(const std::string &url,
              const Headers &request_headers,
              const FormData &form_data,
              Headers &response_headers,
              DataWriter response_body) {
  Response response;
  response.headers = &response_headers;
  response.body = response_body;
  return post_form(url, request_headers, form_data, &response);
}

int post_multipart_form(const std::string &url,
                        const Headers &request_headers,
                        const MultiPartFormData &form_data,
                        Response *response,
                        const Option *option) {
  Request request;
  request.method = METHOD_POST;
  request.url = url;
  request.headers = request_headers;
  std::string boundary;
  std::string request_body = encode_multipart_form(form_data, boundary);
  request.headers.insert(std::make_pair("Content-Type", "multipart/form-data; boundary=" + boundary));
  request.body = buffer_reader(std::move(request_body));
  return send(request, response, option);
}

int post_multipart_form(const std::string &url, const MultiPartFormData &form_data, DataWriter response_body) {
  Response response;
  response.body = response_body;
  return post_multipart_form(url, {}, form_data, &response);
}

int post_multipart_form(const std::string &url,
                        const Headers &request_headers,
                        const MultiPartFormData &form_data,
                        Headers &response_headers,
                        DataWriter response_body) {
  Response response;
  response.headers = &response_headers;
  response.body = response_body;
  return post_multipart_form(url, request_headers, form_data, &response);
}

} // namespace http

} // namespace xl
