#include "url.h"
#include <regex>

namespace {

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

template <typename CharType>
struct CharTypeTrait;
template <>
struct CharTypeTrait<char> {
  static inline const char *URL_PATTERN = "([^:]+)://(?:([^:]+)(?::([^@]+))?@)?([^:/]+)(?::(\\d+))?"
                                          "((?:([^?#]+))?(?:\\?([^#]+)?)?(?:#(.+)?)?)";
  static inline const std::regex URL_REGEXP{URL_PATTERN};
};
template <>
struct CharTypeTrait<wchar_t> {
  static inline const wchar_t *URL_PATTERN = L"([^:]+)://(?:([^:]+)(?::([^@]+))?@)?([^:/]+)(?::(\\d+))?"
                                             L"((?:([^?#]+))?(?:\\?([^#]+)?)?(?:#(.+)?)?)";
  static inline const std::wregex URL_REGEXP{URL_PATTERN};
};

} // namespace

template <typename CharType>
UrlT<CharType> UrlT<CharType>::Parse(const std::basic_string_view<CharType> &url) {
  UrlT<CharType> url_parts;
  std::match_results<typename std::basic_string_view<CharType>::const_iterator> m;
  url_parts.valid = std::regex_match(url.cbegin(), url.cend(), m, CharTypeTrait<CharType>::URL_REGEXP);
  if (url_parts.valid) {
    if (m[Protocol].length() != 0)
      url_parts.protocol = std::basic_string_view<CharType>(&*m[Protocol].first, m[Protocol].length());
    if (m[Username].length() != 0)
      url_parts.username = std::basic_string_view<CharType>(&*m[Username].first, m[Username].length());
    if (m[Password].length() != 0)
      url_parts.password = std::basic_string_view<CharType>(&*m[Password].first, m[Password].length());
    if (m[Domain].length() != 0)
      url_parts.domain = std::basic_string_view<CharType>(&*m[Domain].first, m[Domain].length());
    if (m[Port].length() != 0)
      url_parts.port = std::basic_string_view<CharType>(&*m[Port].first, m[Port].length());
    if (m[FullPath].length() != 0)
      url_parts.full_path = std::basic_string_view<CharType>(&*m[FullPath].first, m[FullPath].length());
    if (m[Path].length() != 0)
      url_parts.path = std::basic_string_view<CharType>(&*m[Path].first, m[Path].length());
    if (m[Query].length() != 0)
      url_parts.query = std::basic_string_view<CharType>(&*m[Query].first, m[Query].length());
    if (m[Fragment].length() != 0)
      url_parts.fragment = std::basic_string_view<CharType>(&*m[Fragment].first, m[Fragment].length());
  }

  return std::move(url_parts);
}

template struct UrlT<char>;
template struct UrlT<wchar_t>;
