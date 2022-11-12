#include "url.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(normal) {
  Url url;
  const char *url_string =
      "http://username:password@domain:80/path/to/page?query=value#fragment";
  bool s = Url::parse(url_string, url);

  BOOST_CHECK_EQUAL(s, true);
  BOOST_CHECK_EQUAL(url.protocol, "http");
  BOOST_CHECK_EQUAL(url.username, "username");
  BOOST_CHECK_EQUAL(url.password, "password");
  BOOST_CHECK_EQUAL(url.domain, "domain");
  BOOST_CHECK_EQUAL(url.port, "80");
  BOOST_CHECK_EQUAL(url.path_with_query_and_fragment,
                    "/path/to/page?query=value#fragment");
  BOOST_CHECK_EQUAL(url.path, "/path/to/page");
  BOOST_CHECK_EQUAL(url.query, "query=value");
  BOOST_CHECK_EQUAL(url.fragment, "fragment");
}

BOOST_AUTO_TEST_CASE(no_password) {
  Url url;
  const char *url_string =
      "http://username@domain:80/path/to/page?query=value#fragment";
  bool s = Url::parse(url_string, url);

  BOOST_CHECK_EQUAL(s, true);
  BOOST_CHECK_EQUAL(url.protocol, "http");
  BOOST_CHECK_EQUAL(url.username, "username");
  BOOST_CHECK_EQUAL(url.password, "");
  BOOST_CHECK_EQUAL(url.domain, "domain");
  BOOST_CHECK_EQUAL(url.port, "80");
  BOOST_CHECK_EQUAL(url.path_with_query_and_fragment,
                    "/path/to/page?query=value#fragment");
  BOOST_CHECK_EQUAL(url.path, "/path/to/page");
  BOOST_CHECK_EQUAL(url.query, "query=value");
  BOOST_CHECK_EQUAL(url.fragment, "fragment");
}

BOOST_AUTO_TEST_CASE(no_username_password) {
  Url url;
  const char *url_string = "http://domain:80/path/to/page?query=value#fragment";
  bool s = Url::parse(url_string, url);

  BOOST_CHECK_EQUAL(s, true);
  BOOST_CHECK_EQUAL(url.protocol, "http");
  BOOST_CHECK_EQUAL(url.username, "");
  BOOST_CHECK_EQUAL(url.password, "");
  BOOST_CHECK_EQUAL(url.domain, "domain");
  BOOST_CHECK_EQUAL(url.port, "80");
  BOOST_CHECK_EQUAL(url.path_with_query_and_fragment,
                    "/path/to/page?query=value#fragment");
  BOOST_CHECK_EQUAL(url.path, "/path/to/page");
  BOOST_CHECK_EQUAL(url.query, "query=value");
  BOOST_CHECK_EQUAL(url.fragment, "fragment");
}

BOOST_AUTO_TEST_CASE(no_port) {
  Url url;
  const char *url_string =
      "http://domain/path/to/page?query=value#fragment";
  bool s = Url::parse(url_string, url);

  BOOST_CHECK_EQUAL(s, true);
  BOOST_CHECK_EQUAL(url.protocol, "http");
  BOOST_CHECK_EQUAL(url.username, "");
  BOOST_CHECK_EQUAL(url.password, "");
  BOOST_CHECK_EQUAL(url.domain, "domain");
  BOOST_CHECK_EQUAL(url.port, "");
  BOOST_CHECK_EQUAL(url.path_with_query_and_fragment,
                    "/path/to/page?query=value#fragment");
  BOOST_CHECK_EQUAL(url.path, "/path/to/page");
  BOOST_CHECK_EQUAL(url.query, "query=value");
  BOOST_CHECK_EQUAL(url.fragment, "fragment");
}

BOOST_AUTO_TEST_CASE(has_query_no_frament) {
  Url url;
  const char *url_string = "http://domain/path/to/page?query=value";
  bool s = Url::parse(url_string, url);

  BOOST_CHECK_EQUAL(s, true);
  BOOST_CHECK_EQUAL(url.protocol, "http");
  BOOST_CHECK_EQUAL(url.username, "");
  BOOST_CHECK_EQUAL(url.password, "");
  BOOST_CHECK_EQUAL(url.domain, "domain");
  BOOST_CHECK_EQUAL(url.port, "");
  BOOST_CHECK_EQUAL(url.path_with_query_and_fragment,
                    "/path/to/page?query=value");
  BOOST_CHECK_EQUAL(url.path, "/path/to/page");
  BOOST_CHECK_EQUAL(url.query, "query=value");
  BOOST_CHECK_EQUAL(url.fragment, "");
}

BOOST_AUTO_TEST_CASE(has_query_no_frament_has_sharp) {
  Url url;
  const char *url_string = "http://domain/path/to/page?query=value#";
  bool s = Url::parse(url_string, url);

  BOOST_CHECK_EQUAL(s, true);
  BOOST_CHECK_EQUAL(url.protocol, "http");
  BOOST_CHECK_EQUAL(url.username, "");
  BOOST_CHECK_EQUAL(url.password, "");
  BOOST_CHECK_EQUAL(url.domain, "domain");
  BOOST_CHECK_EQUAL(url.port, "");
  BOOST_CHECK_EQUAL(url.path_with_query_and_fragment,
                    "/path/to/page?query=value#");
  BOOST_CHECK_EQUAL(url.path, "/path/to/page");
  BOOST_CHECK_EQUAL(url.query, "query=value");
  BOOST_CHECK_EQUAL(url.fragment, "");
}

BOOST_AUTO_TEST_CASE(no_query_has_frament) {
  Url url;
  const char *url_string = "http://domain/path/to/page#fragment";
  bool s = Url::parse(url_string, url);

  BOOST_CHECK_EQUAL(s, true);
  BOOST_CHECK_EQUAL(url.protocol, "http");
  BOOST_CHECK_EQUAL(url.username, "");
  BOOST_CHECK_EQUAL(url.password, "");
  BOOST_CHECK_EQUAL(url.domain, "domain");
  BOOST_CHECK_EQUAL(url.port, "");
  BOOST_CHECK_EQUAL(url.path_with_query_and_fragment, "/path/to/page#fragment");
  BOOST_CHECK_EQUAL(url.path, "/path/to/page");
  BOOST_CHECK_EQUAL(url.query, "");
  BOOST_CHECK_EQUAL(url.fragment, "fragment");
}

BOOST_AUTO_TEST_CASE(no_query_has_question_mask_has_frament) {
  Url url;
  const char *url_string = "http://domain/path/to/page?#fragment";
  bool s = Url::parse(url_string, url);

  BOOST_CHECK_EQUAL(s, true);
  BOOST_CHECK_EQUAL(url.protocol, "http");
  BOOST_CHECK_EQUAL(url.username, "");
  BOOST_CHECK_EQUAL(url.password, "");
  BOOST_CHECK_EQUAL(url.domain, "domain");
  BOOST_CHECK_EQUAL(url.port, "");
  BOOST_CHECK_EQUAL(url.path_with_query_and_fragment,
                    "/path/to/page?#fragment");
  BOOST_CHECK_EQUAL(url.path, "/path/to/page");
  BOOST_CHECK_EQUAL(url.query, "");
  BOOST_CHECK_EQUAL(url.fragment, "fragment");
}

BOOST_AUTO_TEST_CASE(no_query_no_frament) {
  Url url;
  const char *url_string = "http://domain/path/to/page";
  bool s = Url::parse(url_string, url);

  BOOST_CHECK_EQUAL(s, true);
  BOOST_CHECK_EQUAL(url.protocol, "http");
  BOOST_CHECK_EQUAL(url.username, "");
  BOOST_CHECK_EQUAL(url.password, "");
  BOOST_CHECK_EQUAL(url.domain, "domain");
  BOOST_CHECK_EQUAL(url.port, "");
  BOOST_CHECK_EQUAL(url.path_with_query_and_fragment, "/path/to/page");
  BOOST_CHECK_EQUAL(url.path, "/path/to/page");
  BOOST_CHECK_EQUAL(url.query, "");
  BOOST_CHECK_EQUAL(url.fragment, "");
}

BOOST_AUTO_TEST_CASE(root_path) {
  Url url;
  const char *url_string = "http://domain/";
  bool s = Url::parse(url_string, url);

  BOOST_CHECK_EQUAL(s, true);
  BOOST_CHECK_EQUAL(url.protocol, "http");
  BOOST_CHECK_EQUAL(url.username, "");
  BOOST_CHECK_EQUAL(url.password, "");
  BOOST_CHECK_EQUAL(url.domain, "domain");
  BOOST_CHECK_EQUAL(url.port, "");
  BOOST_CHECK_EQUAL(url.path_with_query_and_fragment, "/");
  BOOST_CHECK_EQUAL(url.path, "/");
  BOOST_CHECK_EQUAL(url.query, "");
  BOOST_CHECK_EQUAL(url.fragment, "");
}

BOOST_AUTO_TEST_CASE(no_path) {
  Url url;
  const char *url_string = "http://domain";
  bool s = Url::parse(url_string, url);

  BOOST_CHECK_EQUAL(s, true);
  BOOST_CHECK_EQUAL(url.protocol, "http");
  BOOST_CHECK_EQUAL(url.username, "");
  BOOST_CHECK_EQUAL(url.password, "");
  BOOST_CHECK_EQUAL(url.domain, "domain");
  BOOST_CHECK_EQUAL(url.port, "");
  BOOST_CHECK_EQUAL(url.path_with_query_and_fragment, "");
  BOOST_CHECK_EQUAL(url.path, "");
  BOOST_CHECK_EQUAL(url.query, "");
  BOOST_CHECK_EQUAL(url.fragment, "");
}
