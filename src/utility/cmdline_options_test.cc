
#include "cmdline_options.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(cmdline_options_test)

BOOST_AUTO_TEST_CASE(key_value) {
  const char *argv[] = {"exe_path", "--k=v"};
  auto result = cmdline_options::parse(2, argv);
  std::map<std::string, std::string> expected = {
      {"k", "v"},
  };
  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(key_value_no_value) {
  const char *argv[] = {"exe_path", "--k"};
  auto result = cmdline_options::parse(2, argv);
  std::map<std::string, std::string> expected = {
      {"k", ""},
  };
  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(key_value_no_value_22) {
  const char *argv[] = {"exe_path", "--k="};
  auto result = cmdline_options::parse(2, argv);
  std::map<std::string, std::string> expected = {
      {"k", ""},
  };
  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(short_key_value) {
  const char *argv[] = {"exe_path", "-k", "v"};
  auto result = cmdline_options::parse(3, argv);
  std::map<std::string, std::string> expected = {
      {"k", "v"},
  };
  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(short_key_value_no_value) {
  const char *argv[] = {"exe_path", "-k"};
  auto result = cmdline_options::parse(2, argv);
  std::map<std::string, std::string> expected = {
      {"k", ""},
  };
  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(short_key_value_no_value_2) {
  const char *argv[] = {"exe_path", "-k", "-k2"};
  auto result = cmdline_options::parse(3, argv);
  std::map<std::string, std::string> expected = {
      {"k",  ""},
      {"k2", ""},
  };
  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(short_key_value_no_value_3) {
  const char *argv[] = {"exe_path", "-k", "--k2"};
  auto result = cmdline_options::parse(3, argv);
  std::map<std::string, std::string> expected = {
      {"k",  ""},
      {"k2", ""},
  };
  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(short_key_value_no_value_4) {
  const char *argv[] = {"exe_path", "-k", "--k2=v2"};
  auto result = cmdline_options::parse(3, argv);
  std::map<std::string, std::string> expected = {
      {"k",  ""  },
      {"k2", "v2"},
  };
  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(separeted) {
  const char *argv[] = {"exe_path", "k", "v"};
  auto result = cmdline_options::parse(3, argv);
  std::map<std::string, std::string> expected = {
      {"k", ""},
      {"v", ""},
  };
  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(mixed) {
  const char *argv[] = {"exe_path", "--k1=v1", "-k2", "v2", "k3", "v3"};
  auto result = cmdline_options::parse(6, argv);
  std::map<std::string, std::string> expected = {
      {"k1", "v1"},
      {"k2", "v2"},
      {"k3", ""  },
      {"v3", ""  },
  };
  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(mixed_no_order) {
  const char *argv[] = {"exe_path", "--k1=v1", "k3", "-k2", "v2", "v3"};
  auto result = cmdline_options::parse(6, argv);
  std::map<std::string, std::string> expected = {
      {"k1", "v1"},
      {"k2", "v2"},
      {"k3", ""  },
      {"v3", ""  },
  };
  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(mixed_no_order_no_value) {
  const char *argv[] = {"exe_path", "--k1=v1", "-k2", "-k3", "v3"};
  auto result = cmdline_options::parse(5, argv);
  std::map<std::string, std::string> expected = {
      {"k1", "v1"},
      {"k2", ""  },
      {"k3", "v3"},
  };
  BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_SUITE_END()
