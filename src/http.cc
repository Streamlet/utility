#include <cstring>
#include <utility>
#include <xl/file>
#include <xl/http>

namespace xl {

namespace http {

const char *METHOD_NAME[MethodCount] = {
    "OPTIONS", "HEAD", "GET", "POST", "PUT", "DELETE", "TRACE", "CONNECT",
};

const wchar_t *METHOD_NAME_W[MethodCount] = {
    L"OPTIONS", L"HEAD", L"GET", L"POST", L"PUT", L"DELETE", L"TRACE", L"CONNECT",
};

const char *DEFAULT_USER_AGENT = "Mozilla/5.0 (compatible; MSIE 7.0; Windows NT 10.0; Win64; x64; Trident/7.0) "
                                 "AppleWebKit/500.00 (KHTML, like Gecko) "
                                 "Chrome/100.0.0.0 "
                                 "Safari/500.00 "
                                 "Edg/100.0.0.0";

DataReader BufferReader(const std::string &string_buffer) {
  const char *data_source = &string_buffer[0];
  size_t total_bytes = string_buffer.size();
  size_t bytes_read = 0;
  return [data_source, total_bytes, bytes_read](void *buffer, size_t size, long long *total_size) mutable -> size_t {
    size_t bytes_copy = std::min(size, total_bytes - bytes_read);
    if (bytes_copy > 0) {
      memcpy(buffer, data_source + bytes_read, bytes_copy);
      bytes_read += bytes_copy;
    }
    if (total_size != nullptr) {
      *total_size = total_bytes;
    }
    return bytes_copy;
  };
}

DataReader FileReader(const TCHAR *path) {
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

DataWriter BufferWriter(std::string *string_buffer) {
  return [string_buffer](const void *buffer, size_t size) -> size_t {
    string_buffer->append((const char *)buffer, size);
    return size;
  };
}

DataWriter FileWriter(const TCHAR *path) {
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

void ParseHeader(const std::string &raw_header, Header &parsed_header) {
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

std::string FormDataToBody(const FormData &form_data) {
  return "";
}

std::string MultiPartFormDataToBody(const MultiPartFormData &form_data) {
  return "";
}

int get(const std::string &url, DataWriter response_body) {
  Request request;
  request.method = Get;
  request.url = url;
  Response response;
  response.body = response_body;
  return send(request, &response, nullptr);
}

int get(const std::string &url, const Header &request_header, Header &response_header, DataWriter response_body) {
  Request request;
  request.method = Get;
  request.url = url;
  request.header = request_header;
  Response response;
  response.header = &response_header;
  response.body = response_body;
  return send(request, &response, nullptr);
}

int post(const std::string &url, DataReader request_body, DataWriter response_body) {
  Request request;
  request.method = Post;
  request.url = url;
  request.body = request_body;
  Response response;
  response.body = response_body;
  return send(request, &response, nullptr);
}

int post(const std::string &url,
         const Header &request_header,
         DataReader request_body,
         Header &response_header,
         DataWriter response_body) {
  Request request;
  request.method = Post;
  request.url = url;
  request.header = request_header;
  request.body = request_body;
  Response response;
  response.header = &response_header;
  response.body = response_body;
  return send(request, &response, nullptr);
}

int post_form(const std::string &url,
              const Header &request_header,
              const FormData &form_data,
              Response *response,
              const Option *option) {
  Request request;
  request.method = Post;
  request.url = url;
  request.header = request_header;
  request.body = BufferReader(FormDataToBody(form_data));
  return send(request, response, option);
}

int post_form(const std::string &url, const FormData &form_data, DataWriter response_body) {
  Response response;
  response.body = response_body;
  return post_form(url, {}, form_data, &response);
}

int post_form(const std::string &url,
              const Header &request_header,
              const FormData &form_data,
              Header &response_header,
              DataWriter response_body) {
  Response response;
  response.header = &response_header;
  response.body = response_body;
  return post_form(url, request_header, form_data, &response);
}

int post_multipart_form(const std::string &url,
                        const Header &request_header,
                        const MultiPartFormData form_data,
                        Response *response,
                        const Option *option) {
  Request request;
  request.method = Post;
  request.url = url;
  request.header = request_header;
  request.body = BufferReader(MultiPartFormDataToBody(form_data));
  return send(request, response, option);
}

int post_multipart_form(const std::string &url, const MultiPartFormData form_data, DataWriter response_body) {
  Response response;
  response.body = response_body;
  return post_multipart_form(url, {}, form_data, &response);
}

int post_multipart_form(const std::string &url,
                        const Header &request_header,
                        const MultiPartFormData form_data,
                        Header &response_header,
                        DataWriter response_body) {
  Response response;
  response.header = &response_header;
  response.body = response_body;
  return post_multipart_form(url, request_header, form_data, &response);
}

} // namespace http

} // namespace xl
