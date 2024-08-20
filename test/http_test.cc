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
      if (status == xl::http::StatusOK) {
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
  int status = xl::http::get("http://localhost:8080/echo", xl::http::BufferWriter(&response_body));
  ASSERT_EQ(status, xl::http::StatusCode::StatusOK);
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
                             xl::http::BufferWriter(&response_body));
  ASSERT_EQ(status, xl::http::StatusCode::StatusOK);
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
  int status = xl::http::post("http://localhost:8080/echo", xl::http::BufferReader(request_body),
                              xl::http::BufferWriter(&response_body));
  ASSERT_EQ(status, xl::http::StatusCode::StatusOK);
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
  int status = xl::http::post("http://localhost:8080/echo", request_header, xl::http::BufferReader(request_body),
                              response_header, xl::http::BufferWriter(&response_body));
  ASSERT_EQ(status, xl::http::StatusCode::StatusOK);
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
  int status = xl::http::post_form("http://localhost:8080/echo", form_data, xl::http::BufferWriter(&response_body));
  ASSERT_EQ(status, xl::http::StatusCode::StatusOK);
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
      xl::http::post_multipart_form("http://localhost:8080/echo", form_data, xl::http::BufferWriter(&response_body));
  ASSERT_EQ(status, xl::http::StatusCode::StatusOK);
  printf("%s\n", response_body.c_str());
  HttpEchoResult echo_result;
  ASSERT_EQ(echo_result.json_parse(response_body.c_str()), true);
  ASSERT_EQ(echo_result.version, "HTTP/1.1");
  ASSERT_EQ(echo_result.method, "POST");
  ASSERT_EQ(echo_result.path, "/echo");
  ASSERT_EQ(echo_result.header.find("Host")->second, "localhost:8080");
  ASSERT_EQ(echo_result.header.find("Content-Type")->second,
            "multipart/form-data; boundary=0e46906c66aae1fcbf6443d8d937a5ce");
  ASSERT_EQ(*echo_result.body, "--0e46906c66aae1fcbf6443d8d937a5ce\r\n"
                               "Content-Disposition: form-data; name=\"key1\"\r\n"
                               "\r\n"
                               "value1\r\n"
                               "--0e46906c66aae1fcbf6443d8d937a5ce\r\n"
                               "Content-Disposition: form-data; name=\"key2\"; filename=\"upload.txt\"\r\n"
                               "\r\n"
                               "upload_content\r\n"
                               "--0e46906c66aae1fcbf6443d8d937a5ce\r\n"
                               "Content-Disposition: form-data; name=\"key\\\\3\\\"\"\r\n"
                               "\r\n"
                               "value3\r\n"
                               "--0e46906c66aae1fcbf6443d8d937a5ce--\r\n");
}
