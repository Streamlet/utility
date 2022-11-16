#include <string_view>

struct Url {
  bool parse(std::string url);
  bool parse(const std::string_view &url);

  std::string url;
  std::string_view protocol;
  std::string_view username;
  std::string_view password;
  std::string_view domain;
  std::string_view port;
  std::string_view full_path; // path with query and fragment
  std::string_view path;
  std::string_view query;
  std::string_view fragment;
};
