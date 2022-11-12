#include <string_view>

struct Url {
  static bool parse(const std::string_view &url_string, Url &url);

  std::string_view protocol;
  std::string_view username;
  std::string_view password;
  std::string_view domain;
  std::string_view port;
  std::string_view path_with_query_and_fragment;
  std::string_view path;
  std::string_view query;
  std::string_view fragment;
};
