#include "url.h"
#include <gtest/gtest.h>

TEST(URL_TEST, normal) {
  const char *url_string = "http://username:password@domain:80/path/to/page?query=value#fragment";
  Url url = Url::Parse(url_string);

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

TEST(URL_TEST, no_password) {
  const char *url_string = "http://username@domain:80/path/to/page?query=value#fragment";
  Url url = Url::Parse(url_string);

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

TEST(URL_TEST, no_username_password) {
  const char *url_string = "http://domain:80/path/to/page?query=value#fragment";
  Url url = Url::Parse(url_string);

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

TEST(URL_TEST, no_port) {
  const char *url_string = "http://domain/path/to/page?query=value#fragment";
  Url url = Url::Parse(url_string);

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

TEST(URL_TEST, has_query_no_frament) {
  const char *url_string = "http://domain/path/to/page?query=value";
  Url url = Url::Parse(url_string);

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

TEST(URL_TEST, has_query_no_frament_has_sharp) {
  const char *url_string = "http://domain/path/to/page?query=value#";
  Url url = Url::Parse(url_string);

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

TEST(URL_TEST, no_query_has_frament) {
  const char *url_string = "http://domain/path/to/page#fragment";
  Url url = Url::Parse(url_string);

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

TEST(URL_TEST, no_query_has_question_mask_has_frament) {
  const char *url_string = "http://domain/path/to/page?#fragment";
  Url url = Url::Parse(url_string);

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

TEST(URL_TEST, no_query_no_frament) {
  const char *url_string = "http://domain/path/to/page";
  Url url = Url::Parse(url_string);

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

TEST(URL_TEST, root_path) {
  const char *url_string = "http://domain/";
  Url url = Url::Parse(url_string);

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

TEST(URL_TEST, no_path) {
  const char *url_string = "http://domain";
  Url url = Url::Parse(url_string);

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
