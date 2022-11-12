#include "url.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(normal) {
  Url url;
  const char *url_string = "http://username:password@domain:80"
                           "/path/to/page?query=value#fragment";
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

BOOST_AUTO_TEST_CASE(omitted_username_and_password) {
  Url url;
  const char *url_string = "https://www.domain.com/path/to/page?query=value#fragment";
  bool s = Url::parse(url_string, url);

  BOOST_CHECK_EQUAL(s, true);
  BOOST_CHECK_EQUAL(url.protocol, "https");
  BOOST_CHECK_EQUAL(url.username, "");
  BOOST_CHECK_EQUAL(url.password, "");
  BOOST_CHECK_EQUAL(url.domain, "www.domain.com");
  BOOST_CHECK_EQUAL(url.port, "");
  BOOST_CHECK_EQUAL(url.path_with_query_and_fragment,
                    "/path/to/page?query=value#fragment");
  BOOST_CHECK_EQUAL(url.path, "/path/to/page");
  BOOST_CHECK_EQUAL(url.query, "query=value");
  BOOST_CHECK_EQUAL(url.fragment, "fragment");
}
