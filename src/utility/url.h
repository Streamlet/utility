#include <string>
#include <string_view>

template <typename CharType>
struct UrlT {
  static UrlT Parse(const std::basic_string_view<CharType> &url);

  bool valid = false;
  std::basic_string_view<CharType> protocol;
  std::basic_string_view<CharType> username;
  std::basic_string_view<CharType> password;
  std::basic_string_view<CharType> domain;
  std::basic_string_view<CharType> port;
  std::basic_string_view<CharType> full_path; // path with query and fragment
  std::basic_string_view<CharType> path;
  std::basic_string_view<CharType> query;
  std::basic_string_view<CharType> fragment;
};

using Url = UrlT<char>;
using UrlW = UrlT<wchar_t>;
