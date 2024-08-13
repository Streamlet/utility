#include <regex>
#include <xl/url>

namespace xl {

namespace url {

namespace {

enum url_part : unsigned int {
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
struct char_type_traits;
template <>
struct char_type_traits<char> {
  static const char *URL_PATTERN() {
    return "([^:]+)://(?:([^:]+)(?::([^@]+))?@)?([^:/]+)(?::(\\d+))?"
           "((?:([^?#]+))?(?:\\?([^#]+)?)?(?:#(.+)?)?)";
  }
  static const std::regex URL_REGEXP() {
    return std::regex{URL_PATTERN()};
  };
};
template <>
struct char_type_traits<wchar_t> {
  static const wchar_t *URL_PATTERN() {
    return L"([^:]+)://(?:([^:]+)(?::([^@]+))?@)?([^:/]+)(?::(\\d+))?"
           L"((?:([^?#]+))?(?:\\?([^#]+)?)?(?:#(.+)?)?)";
  }
  static const std::wregex URL_REGEXP() {
    return std::wregex{URL_PATTERN()};
  };
};

template <typename CharType>
url_parts_t<CharType> process_match_result(const std::match_results<const CharType *> &m) {
  url_parts_t<CharType> url_parts = {true};
  if (m[Protocol].length() != 0) {
    url_parts.protocol = basic_string_ref<CharType>(&*m[Protocol].first, m[Protocol].length());
  }
  if (m[Username].length() != 0) {
    url_parts.username = basic_string_ref<CharType>(&*m[Username].first, m[Username].length());
  }
  if (m[Password].length() != 0) {
    url_parts.password = basic_string_ref<CharType>(&*m[Password].first, m[Password].length());
  }
  if (m[Domain].length() != 0) {
    url_parts.domain = basic_string_ref<CharType>(&*m[Domain].first, m[Domain].length());
  }
  if (m[Port].length() != 0) {
    url_parts.port = basic_string_ref<CharType>(&*m[Port].first, m[Port].length());
  }
  if (m[FullPath].length() != 0) {
    url_parts.full_path = basic_string_ref<CharType>(&*m[FullPath].first, m[FullPath].length());
  }
  if (m[Path].length() != 0) {
    url_parts.path = basic_string_ref<CharType>(&*m[Path].first, m[Path].length());
  }
  if (m[Query].length() != 0) {
    url_parts.query = basic_string_ref<CharType>(&*m[Query].first, m[Query].length());
  }
  if (m[Fragment].length() != 0) {
    url_parts.fragment = basic_string_ref<CharType>(&*m[Fragment].first, m[Fragment].length());
  }
  return std::move(url_parts);
}

template <typename CharType>
inline url_parts_t<CharType> parse(const CharType *s) {
  std::match_results<const CharType *> m;
  if (!std::regex_match(s, m, char_type_traits<CharType>::URL_REGEXP())) {
    return url_parts_t<CharType>();
  }
  return process_match_result(m);
}

template <typename CharType>
inline url_parts_t<CharType> parse(const CharType *s, size_t length) {
  std::match_results<const CharType *> m;
  if (!std::regex_match(s, s + length, m, char_type_traits<CharType>::URL_REGEXP())) {
    return url_parts_t<CharType>();
  }
  return process_match_result(m);
}
} // namespace

url_parts parse(const char *s) {
  return parse<>(s);
}

url_parts parse(const char *s, size_t length) {
  return parse<>(s, length);
}

url_parts_w parse(const wchar_t *s) {
  return parse<>(s);
}

url_parts_w parse(const wchar_t *s, size_t length) {
  return parse<>(s, length);
}

} // namespace url

} // namespace xl
