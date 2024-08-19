#include <gtest/gtest.h>
#include <xl/url>

TEST(url_test, normal) {
  const char *url_string = "http://username:password@domain:80/path/to/page?query=value#fragment";
  xl::url_parts url = xl::url::parse(url_string);

  ASSERT_EQ(url.valid, true);
  ASSERT_EQ(url.protocol, "http");
  ASSERT_EQ(url.username, "username");
  ASSERT_EQ(url.password, "password");
  ASSERT_EQ(url.domain, "domain");
  ASSERT_EQ(url.port, "80");
  ASSERT_EQ(url.full_path, "/path/to/page?query=value#fragment");
  ASSERT_EQ(url.path, "/path/to/page");
  ASSERT_EQ(url.query, "query=value");
  ASSERT_EQ(url.fragment, "fragment");
}

TEST(url_test, no_password) {
  const char *url_string = "http://username@domain:80/path/to/page?query=value#fragment";
  xl::url_parts url = xl::url::parse(url_string);

  ASSERT_EQ(url.valid, true);
  ASSERT_EQ(url.protocol, "http");
  ASSERT_EQ(url.username, "username");
  ASSERT_EQ(url.password, "");
  ASSERT_EQ(url.domain, "domain");
  ASSERT_EQ(url.port, "80");
  ASSERT_EQ(url.full_path, "/path/to/page?query=value#fragment");
  ASSERT_EQ(url.path, "/path/to/page");
  ASSERT_EQ(url.query, "query=value");
  ASSERT_EQ(url.fragment, "fragment");
}

TEST(url_test, no_username_password) {
  const char *url_string = "http://domain:80/path/to/page?query=value#fragment";
  xl::url_parts url = xl::url::parse(url_string);

  ASSERT_EQ(url.valid, true);
  ASSERT_EQ(url.protocol, "http");
  ASSERT_EQ(url.username, "");
  ASSERT_EQ(url.password, "");
  ASSERT_EQ(url.domain, "domain");
  ASSERT_EQ(url.port, "80");
  ASSERT_EQ(url.full_path, "/path/to/page?query=value#fragment");
  ASSERT_EQ(url.path, "/path/to/page");
  ASSERT_EQ(url.query, "query=value");
  ASSERT_EQ(url.fragment, "fragment");
}

TEST(url_test, no_port) {
  const char *url_string = "http://domain/path/to/page?query=value#fragment";
  xl::url_parts url = xl::url::parse(url_string);

  ASSERT_EQ(url.valid, true);
  ASSERT_EQ(url.protocol, "http");
  ASSERT_EQ(url.username, "");
  ASSERT_EQ(url.password, "");
  ASSERT_EQ(url.domain, "domain");
  ASSERT_EQ(url.port, "");
  ASSERT_EQ(url.full_path, "/path/to/page?query=value#fragment");
  ASSERT_EQ(url.path, "/path/to/page");
  ASSERT_EQ(url.query, "query=value");
  ASSERT_EQ(url.fragment, "fragment");
}

TEST(url_test, has_query_no_frament) {
  const char *url_string = "http://domain/path/to/page?query=value";
  xl::url_parts url = xl::url::parse(url_string);

  ASSERT_EQ(url.valid, true);
  ASSERT_EQ(url.protocol, "http");
  ASSERT_EQ(url.username, "");
  ASSERT_EQ(url.password, "");
  ASSERT_EQ(url.domain, "domain");
  ASSERT_EQ(url.port, "");
  ASSERT_EQ(url.full_path, "/path/to/page?query=value");
  ASSERT_EQ(url.path, "/path/to/page");
  ASSERT_EQ(url.query, "query=value");
  ASSERT_EQ(url.fragment, "");
}

TEST(url_test, has_query_no_frament_has_sharp) {
  const char *url_string = "http://domain/path/to/page?query=value#";
  xl::url_parts url = xl::url::parse(url_string);

  ASSERT_EQ(url.valid, true);
  ASSERT_EQ(url.protocol, "http");
  ASSERT_EQ(url.username, "");
  ASSERT_EQ(url.password, "");
  ASSERT_EQ(url.domain, "domain");
  ASSERT_EQ(url.port, "");
  ASSERT_EQ(url.full_path, "/path/to/page?query=value#");
  ASSERT_EQ(url.path, "/path/to/page");
  ASSERT_EQ(url.query, "query=value");
  ASSERT_EQ(url.fragment, "");
}

TEST(url_test, no_query_has_frament) {
  const char *url_string = "http://domain/path/to/page#fragment";
  xl::url_parts url = xl::url::parse(url_string);

  ASSERT_EQ(url.valid, true);
  ASSERT_EQ(url.protocol, "http");
  ASSERT_EQ(url.username, "");
  ASSERT_EQ(url.password, "");
  ASSERT_EQ(url.domain, "domain");
  ASSERT_EQ(url.port, "");
  ASSERT_EQ(url.full_path, "/path/to/page#fragment");
  ASSERT_EQ(url.path, "/path/to/page");
  ASSERT_EQ(url.query, "");
  ASSERT_EQ(url.fragment, "fragment");
}

TEST(url_test, no_query_has_question_mask_has_frament) {
  const char *url_string = "http://domain/path/to/page?#fragment";
  xl::url_parts url = xl::url::parse(url_string);

  ASSERT_EQ(url.valid, true);
  ASSERT_EQ(url.protocol, "http");
  ASSERT_EQ(url.username, "");
  ASSERT_EQ(url.password, "");
  ASSERT_EQ(url.domain, "domain");
  ASSERT_EQ(url.port, "");
  ASSERT_EQ(url.full_path, "/path/to/page?#fragment");
  ASSERT_EQ(url.path, "/path/to/page");
  ASSERT_EQ(url.query, "");
  ASSERT_EQ(url.fragment, "fragment");
}

TEST(url_test, no_query_no_frament) {
  const char *url_string = "http://domain/path/to/page";
  xl::url_parts url = xl::url::parse(url_string);

  ASSERT_EQ(url.valid, true);
  ASSERT_EQ(url.protocol, "http");
  ASSERT_EQ(url.username, "");
  ASSERT_EQ(url.password, "");
  ASSERT_EQ(url.domain, "domain");
  ASSERT_EQ(url.port, "");
  ASSERT_EQ(url.full_path, "/path/to/page");
  ASSERT_EQ(url.path, "/path/to/page");
  ASSERT_EQ(url.query, "");
  ASSERT_EQ(url.fragment, "");
}

TEST(url_test, root_path) {
  const char *url_string = "http://domain/";
  xl::url_parts url = xl::url::parse(url_string);

  ASSERT_EQ(url.valid, true);
  ASSERT_EQ(url.protocol, "http");
  ASSERT_EQ(url.username, "");
  ASSERT_EQ(url.password, "");
  ASSERT_EQ(url.domain, "domain");
  ASSERT_EQ(url.port, "");
  ASSERT_EQ(url.full_path, "/");
  ASSERT_EQ(url.path, "/");
  ASSERT_EQ(url.query, "");
  ASSERT_EQ(url.fragment, "");
}

TEST(url_test, no_path) {
  const char *url_string = "http://domain";
  xl::url_parts url = xl::url::parse(url_string);

  ASSERT_EQ(url.valid, true);
  ASSERT_EQ(url.protocol, "http");
  ASSERT_EQ(url.username, "");
  ASSERT_EQ(url.password, "");
  ASSERT_EQ(url.domain, "domain");
  ASSERT_EQ(url.port, "");
  ASSERT_EQ(url.full_path, "");
  ASSERT_EQ(url.path, "");
  ASSERT_EQ(url.query, "");
  ASSERT_EQ(url.fragment, "");
}

TEST(url_test, url_encode) {
  ASSERT_EQ(xl::url::encode("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-._~"),
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-._~");
  ASSERT_EQ(xl::url::encode("+% ="), "%2B%25%20%3D");
  ASSERT_EQ(xl::url::encode("`!@#$%^&*()+[]{}\\|;:'\",<>/?"),
            "%60%21%40%23%24%25%5E%26%2A%28%29%2B%5B%5D%7B%7D%5C%7C%3B%3A%27%22%2C%3C%3E%2F%3F");
  ASSERT_EQ(xl::url::encode("你好"), "%E4%BD%A0%E5%A5%BD");
}

TEST(url_test, url_decode) {
  ASSERT_EQ(xl::url::decode("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-._~"),
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-._~");
  ASSERT_EQ(xl::url::decode("%2B%25%20%3D%2b%25%20%3d"), "+% =+% =");
  ASSERT_EQ(xl::url::decode("%60%21%40%23%24%25%5E%26%2A%28%29%2B%5B%5D%7B%7D%5C%7C%3B%3A%27%22%2C%3C%3E%2F%3F"
                            "%60%21%40%23%24%25%5e%26%2a%28%29%2b%5b%5d%7b%7d%5c%7c%3b%3a%27%22%2c%3c%3e%2f%3f"),
            "`!@#$%^&*()+[]{}\\|;:'\",<>/?"
            "`!@#$%^&*()+[]{}\\|;:'\",<>/?");
  ASSERT_EQ(xl::url::decode("%E4%BD%A0%E5%A5%BD"
                            "%e4%bd%a0%e5%a5%bd"),
            "你好"
            "你好");
  ASSERT_EQ(xl::url::decode("%30%31%32%33%34%35%36%37%38%39"
                            "%41%42%43%44%45%46%47%48%49%4A%4B%4C%4D%4E%4F%50%51%52%53%54%55%56%57%58%59%5A"
                            "%61%62%63%64%65%66%67%68%69%6A%6B%6C%6D%6E%6F%70%71%72%73%74%75%76%77%78%79%7A"
                            "%2D%2E%5F%7E"
                            "%30%31%32%33%34%35%36%37%38%39"
                            "%41%42%43%44%45%46%47%48%49%4a%4b%4c%4d%4e%4f%50%51%52%53%54%55%56%57%58%59%5a"
                            "%61%62%63%64%65%66%67%68%69%6a%6b%6c%6d%6e%6f%70%71%72%73%74%75%76%77%78%79%7a"
                            "%2d%2e%5f%7e"),
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-._~"
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-._~");
}
