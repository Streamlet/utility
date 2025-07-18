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

#include <gtest/gtest.h>
#include <xl/file>
#include <xl/http>
#include <xl/json>
#include <xl/process>
#include <xl/scope_exit>

class http_test : public ::testing::Test {
protected:
  static void SetUpTestCase() {
    auto workdir = xl::path::dirname(xl::process::executable_path().c_str());
    pid_ = xl::process::start(_T("python"), {_T("http_echo.py")}, workdir);
    printf("Waiting for HTTP server ready...\n");
    while (true) {
      int status = xl::http::get("http://localhost:8080/", nullptr);
      if (status == xl::http::STATUS_OK) {
        break;
      }
      xl::process::sleep(100);
    }
  }

  static void TearDownTestCase() {
    xl::process::kill(pid_);
  }

  static int pid_;
};

int http_test::pid_ = 0;

XL_JSON_BEGIN(HttpEchoResult)
  XL_JSON_MEMBER(std::string, version)
  XL_JSON_MEMBER(std::string, method)
  XL_JSON_MEMBER(std::string, path)
  XL_JSON_MEMBER(xl::http::Headers, header)
  XL_JSON_MEMBER(std::unique_ptr<std::string>, body)
XL_JSON_END()

TEST_F(http_test, simple_get) {
  std::string response_body;
  int status = xl::http::get("http://localhost:8080/echo", xl::http::buffer_writer(&response_body));
  ASSERT_EQ(status, xl::http::STATUS_OK);
  HttpEchoResult echo_result;
  ASSERT_EQ(echo_result.json_parse(response_body.c_str()), true);
  ASSERT_EQ(echo_result.version, "HTTP/1.1");
  ASSERT_EQ(echo_result.method, "GET");
  ASSERT_EQ(echo_result.path, "/echo");
  ASSERT_EQ(echo_result.header.find("Host")->second, "localhost:8080");
  ASSERT_EQ(echo_result.body, nullptr);
}

TEST_F(http_test, get_with_header) {
  xl::http::Headers request_header = {
      {"TestCase", "get_with_header"}
  };
  xl::http::Headers response_header;
  std::string response_body;
  int status = xl::http::get("http://localhost:8080/echo", request_header, response_header,
                             xl::http::buffer_writer(&response_body));
  ASSERT_EQ(status, xl::http::STATUS_OK);
  HttpEchoResult echo_result;
  ASSERT_EQ(echo_result.json_parse(response_body.c_str()), true);
  ASSERT_EQ(echo_result.version, "HTTP/1.1");
  ASSERT_EQ(echo_result.method, "GET");
  ASSERT_EQ(echo_result.path, "/echo");
  ASSERT_EQ(echo_result.header.find("Host")->second, "localhost:8080");
  ASSERT_EQ(echo_result.header.find("TestCase")->second, "get_with_header");
  ASSERT_EQ(echo_result.body, nullptr);
  ASSERT_EQ(response_header.find("EchoServerVersion")->second, "1.0");
  ASSERT_EQ(response_header.find("Content-Type")->second, "application/json");
}

TEST_F(http_test, simple_post) {
  std::string request_body = "request_body_content";
  std::string response_body;
  int status = xl::http::post("http://localhost:8080/echo", xl::http::buffer_reader(request_body),
                              xl::http::buffer_writer(&response_body));
  ASSERT_EQ(status, xl::http::STATUS_OK);
  HttpEchoResult echo_result;
  ASSERT_EQ(echo_result.json_parse(response_body.c_str()), true);
  ASSERT_EQ(echo_result.version, "HTTP/1.1");
  ASSERT_EQ(echo_result.method, "POST");
  ASSERT_EQ(echo_result.path, "/echo");
  ASSERT_EQ(echo_result.header.find("Host")->second, "localhost:8080");
  ASSERT_EQ(*echo_result.body, "request_body_content");
}

TEST_F(http_test, post_with_header) {
  xl::http::Headers request_header = {
      {"TestCase", "post_with_header"}
  };
  xl::http::Headers response_header;
  std::string request_body = "request_body_content";
  std::string response_body;
  int status = xl::http::post("http://localhost:8080/echo", request_header, xl::http::buffer_reader(request_body),
                              response_header, xl::http::buffer_writer(&response_body));
  ASSERT_EQ(status, xl::http::STATUS_OK);
  HttpEchoResult echo_result;
  ASSERT_EQ(echo_result.json_parse(response_body.c_str()), true);
  ASSERT_EQ(echo_result.version, "HTTP/1.1");
  ASSERT_EQ(echo_result.method, "POST");
  ASSERT_EQ(echo_result.path, "/echo");
  ASSERT_EQ(echo_result.header.find("Host")->second, "localhost:8080");
  ASSERT_EQ(echo_result.header.find("TestCase")->second, "post_with_header");
  ASSERT_EQ(*echo_result.body, "request_body_content");
  ASSERT_EQ(response_header.find("EchoServerVersion")->second, "1.0");
  ASSERT_EQ(response_header.find("Content-Type")->second, "application/json");
}

TEST_F(http_test, post_form) {
  xl::http::FormData form_data = {
      {"key1",  "value1" },
      {"key2",  "value2" },
      {"key3&", "value3="},
  };
  std::string response_body;
  int status = xl::http::post_form("http://localhost:8080/echo", form_data, xl::http::buffer_writer(&response_body));
  ASSERT_EQ(status, xl::http::STATUS_OK);
  printf("%s\n", response_body.c_str());
  HttpEchoResult echo_result;
  ASSERT_EQ(echo_result.json_parse(response_body.c_str()), true);
  ASSERT_EQ(echo_result.version, "HTTP/1.1");
  ASSERT_EQ(echo_result.method, "POST");
  ASSERT_EQ(echo_result.path, "/echo");
  ASSERT_EQ(echo_result.header.find("Host")->second, "localhost:8080");
  ASSERT_EQ(echo_result.header.find("Content-Type")->second, "application/x-www-form-urlencoded");
  ASSERT_EQ(*echo_result.body, "key1=value1&key2=value2&key3%26=value3%3D");
}

TEST_F(http_test, post_multipart_form) {
  ASSERT_EQ(xl::file::write(_T("upload.txt"), "upload_content"), true);
  XL_ON_BLOCK_EXIT(xl::fs::remove, _T("upload.txt"));
  xl::http::MultiPartFormData form_data = {
      {"key1",     {"value1"}            },
      {"key2",     {"", _T("upload.txt")}},
      {"key\\3\"", {"value3"}            },
  };
  std::string response_body;
  int status =
      xl::http::post_multipart_form("http://localhost:8080/echo", form_data, xl::http::buffer_writer(&response_body));
  ASSERT_EQ(status, xl::http::STATUS_OK);
  HttpEchoResult echo_result;
  ASSERT_EQ(echo_result.json_parse(response_body.c_str()), true);
  ASSERT_EQ(echo_result.version, "HTTP/1.1");
  ASSERT_EQ(echo_result.method, "POST");
  ASSERT_EQ(echo_result.path, "/echo");
  ASSERT_EQ(echo_result.header.find("Host")->second, "localhost:8080");
  std::string boundary = xl::string::split(echo_result.header.find("Content-Type")->second, "=", 2)[1];
  ASSERT_EQ(*echo_result.body, "--" + boundary + "\r\n"
                               "Content-Disposition: form-data; name=\"key1\"\r\n"
                               "\r\n"
                               "value1\r\n"
                               "--" + boundary + "\r\n"
                               "Content-Disposition: form-data; name=\"key2\"; filename=\"upload.txt\"\r\n"
                               "\r\n"
                               "upload_content\r\n"
                               "--" + boundary + "\r\n"
                               "Content-Disposition: form-data; name=\"key\\\\3\\\"\"\r\n"
                               "\r\n"
                               "value3\r\n"
                               "--" + boundary + "--\r\n");
}
