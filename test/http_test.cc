#include <gtest/gtest.h>
#include <xl/http>
#include <xl/json>

XL_JSON_BEGIN(HttpEchoResult)
  XL_JSON_MEMBER(std::string, version)
  XL_JSON_MEMBER(std::string, method)
  XL_JSON_MEMBER(std::string, path)
  XL_JSON_MEMBER(xl::http::Headers, header)
  XL_JSON_MEMBER(std::unique_ptr<std::string>, body)
XL_JSON_END()

TEST(http_test, simple_get) {
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

TEST(http_test, get_with_header) {
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

TEST(http_test, simple_post) {
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

TEST(http_test, post_with_header) {
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
