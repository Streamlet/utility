#include "url.h"
#include <regex>

namespace {

static const char *URL_PATTERN =
    "([^:]+)://(?:([^:]+)(?::([^@]+))?@)?([^:/]+)(?::(\\d+))?"
    "((?:([^?#]+))?(?:\\?([^#]+))?(?:#(.+))?)";

enum UrlPart : unsigned int {
  Protocol = 1,
  Username = 2,
  Password = 3,
  Domain = 4,
  Port = 5,
  PathWithQueryAndFragment = 6,
  Path = 7,
  Query = 8,
  Fragment = 9,
};

} // namespace

bool Url::parse(const std::string_view &url_string, Url &url) {
  static std::regex re(URL_PATTERN);

  std::match_results<std::string_view::const_iterator> m;
  if (!std::regex_match(url_string.cbegin(), url_string.cend(), m, re)) {
    return false;
  }

  url.protocol = std::string_view(&*m[UrlPart::Protocol].first,
                                  m[UrlPart::Protocol].length());
  url.username = std::string_view(&*m[UrlPart::Username].first,
                                  m[UrlPart::Username].length());
  url.password = std::string_view(&*m[UrlPart::Password].first,
                                  m[UrlPart::Password].length());
  url.domain =
      std::string_view(&*m[UrlPart::Domain].first, m[UrlPart::Domain].length());
  url.port =
      std::string_view(&*m[UrlPart::Port].first, m[UrlPart::Port].length());
  url.path_with_query_and_fragment =
      std::string_view(&*m[UrlPart::PathWithQueryAndFragment].first,
                       m[UrlPart::PathWithQueryAndFragment].length());
  url.path =
      std::string_view(&*m[UrlPart::Path].first, m[UrlPart::Path].length());
  url.query =
      std::string_view(&*m[UrlPart::Query].first, m[UrlPart::Query].length());
  url.fragment = std::string_view(&*m[UrlPart::Fragment].first,
                                  m[UrlPart::Fragment].length());

  return true;
}
