#include <gtest/gtest.h>
#include <xl/http>
#include <xl/json>

XL_JSON_BEGIN(HttpEchoResult)
  XL_JSON_MEMBER(std::string, version)
  XL_JSON_MEMBER(std::string, method)
  XL_JSON_MEMBER(std::string, path)
  XL_JSON_MEMBER(xl::http::Header, header)
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
}