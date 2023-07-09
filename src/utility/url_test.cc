#include "url.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(url_test)

BOOST_AUTO_TEST_CASE(normal) {
  const char *url_string = "http://username:password@domain:80/path/to/page?query=value#fragment";
  Url url = Url::Parse(url_string);

  BOOST_CHECK(url.valid == true);
  BOOST_CHECK(url.protocol == "http");
  BOOST_CHECK(url.username == "username");
  BOOST_CHECK(url.password == "password");
  BOOST_CHECK(url.domain == "domain");
  BOOST_CHECK(url.port == "80");
  BOOST_CHECK(url.full_path == "/path/to/page?query=value#fragment");
  BOOST_CHECK(url.path == "/path/to/page");
  BOOST_CHECK(url.query == "query=value");
  BOOST_CHECK(url.fragment == "fragment");
}

BOOST_AUTO_TEST_CASE(no_password) {
  const char *url_string = "http://username@domain:80/path/to/page?query=value#fragment";
  Url url = Url::Parse(url_string);

  BOOST_CHECK(url.valid == true);
  BOOST_CHECK(url.protocol == "http");
  BOOST_CHECK(url.username == "username");
  BOOST_CHECK(url.password == "");
  BOOST_CHECK(url.domain == "domain");
  BOOST_CHECK(url.port == "80");
  BOOST_CHECK(url.full_path == "/path/to/page?query=value#fragment");
  BOOST_CHECK(url.path == "/path/to/page");
  BOOST_CHECK(url.query == "query=value");
  BOOST_CHECK(url.fragment == "fragment");
}

BOOST_AUTO_TEST_CASE(no_username_password) {
  const char *url_string = "http://domain:80/path/to/page?query=value#fragment";
  Url url = Url::Parse(url_string);

  BOOST_CHECK(url.valid == true);
  BOOST_CHECK(url.protocol == "http");
  BOOST_CHECK(url.username == "");
  BOOST_CHECK(url.password == "");
  BOOST_CHECK(url.domain == "domain");
  BOOST_CHECK(url.port == "80");
  BOOST_CHECK(url.full_path == "/path/to/page?query=value#fragment");
  BOOST_CHECK(url.path == "/path/to/page");
  BOOST_CHECK(url.query == "query=value");
  BOOST_CHECK(url.fragment == "fragment");
}

BOOST_AUTO_TEST_CASE(no_port) {
  const char *url_string = "http://domain/path/to/page?query=value#fragment";
  Url url = Url::Parse(url_string);

  BOOST_CHECK(url.valid == true);
  BOOST_CHECK(url.protocol == "http");
  BOOST_CHECK(url.username == "");
  BOOST_CHECK(url.password == "");
  BOOST_CHECK(url.domain == "domain");
  BOOST_CHECK(url.port == "");
  BOOST_CHECK(url.full_path == "/path/to/page?query=value#fragment");
  BOOST_CHECK(url.path == "/path/to/page");
  BOOST_CHECK(url.query == "query=value");
  BOOST_CHECK(url.fragment == "fragment");
}

BOOST_AUTO_TEST_CASE(has_query_no_frament) {
  const char *url_string = "http://domain/path/to/page?query=value";
  Url url = Url::Parse(url_string);

  BOOST_CHECK(url.valid == true);
  BOOST_CHECK(url.protocol == "http");
  BOOST_CHECK(url.username == "");
  BOOST_CHECK(url.password == "");
  BOOST_CHECK(url.domain == "domain");
  BOOST_CHECK(url.port == "");
  BOOST_CHECK(url.full_path == "/path/to/page?query=value");
  BOOST_CHECK(url.path == "/path/to/page");
  BOOST_CHECK(url.query == "query=value");
  BOOST_CHECK(url.fragment == "");
}

BOOST_AUTO_TEST_CASE(has_query_no_frament_has_sharp) {
  const char *url_string = "http://domain/path/to/page?query=value#";
  Url url = Url::Parse(url_string);

  BOOST_CHECK(url.valid == true);
  BOOST_CHECK(url.protocol == "http");
  BOOST_CHECK(url.username == "");
  BOOST_CHECK(url.password == "");
  BOOST_CHECK(url.domain == "domain");
  BOOST_CHECK(url.port == "");
  BOOST_CHECK(url.full_path == "/path/to/page?query=value#");
  BOOST_CHECK(url.path == "/path/to/page");
  BOOST_CHECK(url.query == "query=value");
  BOOST_CHECK(url.fragment == "");
}

BOOST_AUTO_TEST_CASE(no_query_has_frament) {
  const char *url_string = "http://domain/path/to/page#fragment";
  Url url = Url::Parse(url_string);

  BOOST_CHECK(url.valid == true);
  BOOST_CHECK(url.protocol == "http");
  BOOST_CHECK(url.username == "");
  BOOST_CHECK(url.password == "");
  BOOST_CHECK(url.domain == "domain");
  BOOST_CHECK(url.port == "");
  BOOST_CHECK(url.full_path == "/path/to/page#fragment");
  BOOST_CHECK(url.path == "/path/to/page");
  BOOST_CHECK(url.query == "");
  BOOST_CHECK(url.fragment == "fragment");
}

BOOST_AUTO_TEST_CASE(no_query_has_question_mask_has_frament) {
  const char *url_string = "http://domain/path/to/page?#fragment";
  Url url = Url::Parse(url_string);

  BOOST_CHECK(url.valid == true);
  BOOST_CHECK(url.protocol == "http");
  BOOST_CHECK(url.username == "");
  BOOST_CHECK(url.password == "");
  BOOST_CHECK(url.domain == "domain");
  BOOST_CHECK(url.port == "");
  BOOST_CHECK(url.full_path == "/path/to/page?#fragment");
  BOOST_CHECK(url.path == "/path/to/page");
  BOOST_CHECK(url.query == "");
  BOOST_CHECK(url.fragment == "fragment");
}

BOOST_AUTO_TEST_CASE(no_query_no_frament) {
  const char *url_string = "http://domain/path/to/page";
  Url url = Url::Parse(url_string);

  BOOST_CHECK(url.valid == true);
  BOOST_CHECK(url.protocol == "http");
  BOOST_CHECK(url.username == "");
  BOOST_CHECK(url.password == "");
  BOOST_CHECK(url.domain == "domain");
  BOOST_CHECK(url.port == "");
  BOOST_CHECK(url.full_path == "/path/to/page");
  BOOST_CHECK(url.path == "/path/to/page");
  BOOST_CHECK(url.query == "");
  BOOST_CHECK(url.fragment == "");
}

BOOST_AUTO_TEST_CASE(root_path) {
  const char *url_string = "http://domain/";
  Url url = Url::Parse(url_string);

  BOOST_CHECK(url.valid == true);
  BOOST_CHECK(url.protocol == "http");
  BOOST_CHECK(url.username == "");
  BOOST_CHECK(url.password == "");
  BOOST_CHECK(url.domain == "domain");
  BOOST_CHECK(url.port == "");
  BOOST_CHECK(url.full_path == "/");
  BOOST_CHECK(url.path == "/");
  BOOST_CHECK(url.query == "");
  BOOST_CHECK(url.fragment == "");
}

BOOST_AUTO_TEST_CASE(no_path) {
  const char *url_string = "http://domain";
  Url url = Url::Parse(url_string);

  BOOST_CHECK(url.valid == true);
  BOOST_CHECK(url.protocol == "http");
  BOOST_CHECK(url.username == "");
  BOOST_CHECK(url.password == "");
  BOOST_CHECK(url.domain == "domain");
  BOOST_CHECK(url.port == "");
  BOOST_CHECK(url.full_path == "");
  BOOST_CHECK(url.path == "");
  BOOST_CHECK(url.query == "");
  BOOST_CHECK(url.fragment == "");
}

BOOST_AUTO_TEST_SUITE_END()
