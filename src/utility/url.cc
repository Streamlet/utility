#include "url.h"
#include <regex>

namespace {

static const char *URL_PATTERN =
    "([^:]+)://(?:([^:]+)(?::([^@]+))?@)?([^:/]+)(?::(\\d+))?"
    "((?:([^?#]+))?(?:\\?([^#]+)?)?(?:#(.+)?)?)";

enum UrlPart : unsigned int {
  Protocol = 1,
  Username = 2,
  Password = 3,
  Domain = 4,
  Port = 5,
  FullPath = 6,
  Path = 7,
  Query = 8,
  Fragment = 9,
};

} // namespace

bool Url::parse(std::string url) {
  this->url = std::move(url);
  return parse(std::string_view(this->url));
}

bool Url::parse(const std::string_view &url) {
  static std::regex re(URL_PATTERN);

  std::match_results<std::string_view::const_iterator> m;
  if (!std::regex_match(url.cbegin(), url.cend(), m, re)) {
    return false;
  }

  protocol = m[Protocol].length() == 0
                 ? ""
                 : std::string_view(&*m[Protocol].first, m[Protocol].length());
  username = m[Username].length() == 0
                 ? ""
                 : std::string_view(&*m[Username].first, m[Username].length());
  password = m[Password].length() == 0
                 ? ""
                 : std::string_view(&*m[Password].first, m[Password].length());
  domain = m[Domain].length() == 0
               ? ""
               : std::string_view(&*m[Domain].first, m[Domain].length());
  port = m[Port].length() == 0
             ? ""
             : std::string_view(&*m[Port].first, m[Port].length());
  full_path = m[FullPath].length() == 0
                  ? ""
                  : std::string_view(&*m[FullPath].first, m[FullPath].length());
  path = m[Path].length() == 0
             ? ""
             : std::string_view(&*m[Path].first, m[Path].length());
  query = m[Query].length() == 0
              ? ""
              : std::string_view(&*m[Query].first, m[Query].length());
  fragment = m[Fragment].length() == 0
                 ? ""
                 : std::string_view(&*m[Fragment].first, m[Fragment].length());

  return true;
}
