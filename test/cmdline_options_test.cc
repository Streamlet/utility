
#include <gtest/gtest.h>
#include <xl/cmdline_options.h>
#include <xl/native_string.h>

TEST(cmdline_options_test, key_value) {
  const TCHAR *argv[] = {_T("exe_path"), _T("--k=v")};
  auto result = xl::cmdline_options::parse(2, argv);
  std::map<xl::native_string, xl::native_string> expected = {
      {_T("k"), _T("v")},
  };
  ASSERT_EQ(result.parsed_map, expected);
}

TEST(cmdline_options_test, key_value_no_equal) {
  const TCHAR *argv[] = {_T("exe_path"), _T("--k"), _T("v")};
  auto result = xl::cmdline_options::parse(3, argv);
  std::map<xl::native_string, xl::native_string> expected = {
      {_T("k"), _T("v")},
  };
  ASSERT_EQ(result.parsed_map, expected);
}

TEST(cmdline_options_test, key_value_no_value) {
  const TCHAR *argv[] = {_T("exe_path"), _T("--k")};
  auto result = xl::cmdline_options::parse(2, argv);
  std::map<xl::native_string, xl::native_string> expected = {
      {_T("k"), _T("")},
  };
  ASSERT_EQ(result.parsed_map, expected);
}

TEST(cmdline_options_test, key_value_no_value_22) {
  const TCHAR *argv[] = {_T("exe_path"), _T("--k=")};
  auto result = xl::cmdline_options::parse(2, argv);
  std::map<xl::native_string, xl::native_string> expected = {
      {_T("k"), _T("")},
  };
  ASSERT_EQ(result.parsed_map, expected);
}

TEST(cmdline_options_test, short_key_value) {
  const TCHAR *argv[] = {_T("exe_path"), _T("-k"), _T("v")};
  auto result = xl::cmdline_options::parse(3, argv);
  std::map<xl::native_string, xl::native_string> expected = {
      {_T("k"), _T("v")},
  };
  ASSERT_EQ(result.parsed_map, expected);
}

TEST(cmdline_options_test, short_key_value_no_value) {
  const TCHAR *argv[] = {_T("exe_path"), _T("-k")};
  auto result = xl::cmdline_options::parse(2, argv);
  std::map<xl::native_string, xl::native_string> expected = {
      {_T("k"), _T("")},
  };
  ASSERT_EQ(result.parsed_map, expected);
}

TEST(cmdline_options_test, short_key_value_no_value_2) {
  const TCHAR *argv[] = {_T("exe_path"), _T("-k"), _T("-k2")};
  auto result = xl::cmdline_options::parse(3, argv);
  std::map<xl::native_string, xl::native_string> expected = {
      {_T("k"),  _T("")},
      {_T("k2"), _T("")},
  };
  ASSERT_EQ(result.parsed_map, expected);
}

TEST(cmdline_options_test, short_key_value_no_value_3) {
  const TCHAR *argv[] = {_T("exe_path"), _T("-k"), _T("--k2")};
  auto result = xl::cmdline_options::parse(3, argv);
  std::map<xl::native_string, xl::native_string> expected = {
      {_T("k"),  _T("")},
      {_T("k2"), _T("")},
  };
  ASSERT_EQ(result.parsed_map, expected);
}

TEST(cmdline_options_test, short_key_value_no_value_4) {
  const TCHAR *argv[] = {_T("exe_path"), _T("-k"), _T("--k2=v2")};
  auto result = xl::cmdline_options::parse(3, argv);
  std::map<xl::native_string, xl::native_string> expected = {
      {_T("k"),  _T("")  },
      {_T("k2"), _T("v2")},
  };
  ASSERT_EQ(result.parsed_map, expected);
}

TEST(cmdline_options_test, separeted) {
  const TCHAR *argv[] = {_T("exe_path"), _T("k"), _T("v")};
  auto result = xl::cmdline_options::parse(3, argv);
  std::map<xl::native_string, xl::native_string> expected = {
      {_T("k"), _T("")},
      {_T("v"), _T("")},
  };
  ASSERT_EQ(result.parsed_map, expected);
}

TEST(cmdline_options_test, mixed) {
  const TCHAR *argv[] = {_T("exe_path"), _T("--k1=v1"), _T("-k2"), _T("v2"), _T("k3"), _T("v3")};
  auto result = xl::cmdline_options::parse(6, argv);
  std::map<xl::native_string, xl::native_string> expected = {
      {_T("k1"), _T("v1")},
      {_T("k2"), _T("v2")},
      {_T("k3"), _T("")  },
      {_T("v3"), _T("")  },
  };
  ASSERT_EQ(result.parsed_map, expected);
}

TEST(cmdline_options_test, mixed_no_order) {
  const TCHAR *argv[] = {_T("exe_path"), _T("--k1=v1"), _T("k3"), _T("-k2"), _T("v2"), _T("v3")};
  auto result = xl::cmdline_options::parse(6, argv);
  std::map<xl::native_string, xl::native_string> expected = {
      {_T("k1"), _T("v1")},
      {_T("k2"), _T("v2")},
      {_T("k3"), _T("")  },
      {_T("v3"), _T("")  },
  };
  ASSERT_EQ(result.parsed_map, expected);
}

TEST(cmdline_options_test, mixed_no_order_no_value) {
  const TCHAR *argv[] = {_T("exe_path"), _T("--k1=v1"), _T("-k2"), _T("-k3"), _T("v3")};
  auto result = xl::cmdline_options::parse(5, argv);
  std::map<xl::native_string, xl::native_string> expected = {
      {_T("k1"), _T("v1")},
      {_T("k2"), _T("")  },
      {_T("k3"), _T("v3")},
  };
  ASSERT_EQ(result.parsed_map, expected);
}

TEST(cmdline_options_test, case_translate) {
  const TCHAR *argv[] = {_T("exe_path"), _T("--k1=v1"), _T("-k2"), _T("1"), _T("k3")};
  auto result = xl::cmdline_options::parse(5, argv);
  ASSERT_EQ(result.has(_T("k1")), true);
  ASSERT_EQ(result.has(_T("k2")), true);
  ASSERT_EQ(result.has(_T("k3")), true);
  ASSERT_EQ(result.has(_T("k4")), false);
  ASSERT_EQ(result.get(_T("k1")), _T("v1"));
  ASSERT_EQ(result.get(_T("k2")), _T("1"));
  ASSERT_EQ(result.get(_T("k3")), _T(""));
  ASSERT_EQ(result.get(_T("k4")), _T(""));
  ASSERT_EQ(result.get_as<int>(_T("k1")), 0);
  ASSERT_EQ(result.get_as<int>(_T("k2")), 1);
  ASSERT_EQ(result.get_as<int>(_T("k3")), 0);
  ASSERT_EQ(result.get_as<int>(_T("k4")), 0);
  ASSERT_EQ(result.get_as<bool>(_T("k1")), false);
  ASSERT_EQ(result.get_as<bool>(_T("k2")), true);
  ASSERT_EQ(result.get_as<bool>(_T("k3")), false);
  ASSERT_EQ(result.get_as<bool>(_T("k4")), false);
}
