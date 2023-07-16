
#include "cmdline_options.h"
#include "native_string.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(cmdline_options_test)

BOOST_AUTO_TEST_CASE(key_value) {
  const TCHAR *argv[] = {_T("exe_path"), _T("--k=v")};
  auto result = cmdline_options::parse(2, argv);
  std::map<std::basic_string<TCHAR>, std::basic_string<TCHAR>> expected = {
      {_T("k"), _T("v")},
  };
  BOOST_CHECK(result.parsed_map == expected);
}

BOOST_AUTO_TEST_CASE(key_value_no_value) {
  const TCHAR *argv[] = {_T("exe_path"), _T("--k")};
  auto result = cmdline_options::parse(2, argv);
  std::map<std::basic_string<TCHAR>, std::basic_string<TCHAR>> expected = {
      {_T("k"), _T("")},
  };
  BOOST_CHECK(result.parsed_map == expected);
}

BOOST_AUTO_TEST_CASE(key_value_no_value_22) {
  const TCHAR *argv[] = {_T("exe_path"), _T("--k=")};
  auto result = cmdline_options::parse(2, argv);
  std::map<std::basic_string<TCHAR>, std::basic_string<TCHAR>> expected = {
      {_T("k"), _T("")},
  };
  BOOST_CHECK(result.parsed_map == expected);
}

BOOST_AUTO_TEST_CASE(short_key_value) {
  const TCHAR *argv[] = {_T("exe_path"), _T("-k"), _T("v")};
  auto result = cmdline_options::parse(3, argv);
  std::map<std::basic_string<TCHAR>, std::basic_string<TCHAR>> expected = {
      {_T("k"), _T("v")},
  };
  BOOST_CHECK(result.parsed_map == expected);
}

BOOST_AUTO_TEST_CASE(short_key_value_no_value) {
  const TCHAR *argv[] = {_T("exe_path"), _T("-k")};
  auto result = cmdline_options::parse(2, argv);
  std::map<std::basic_string<TCHAR>, std::basic_string<TCHAR>> expected = {
      {_T("k"), _T("")},
  };
  BOOST_CHECK(result.parsed_map == expected);
}

BOOST_AUTO_TEST_CASE(short_key_value_no_value_2) {
  const TCHAR *argv[] = {_T("exe_path"), _T("-k"), _T("-k2")};
  auto result = cmdline_options::parse(3, argv);
  std::map<std::basic_string<TCHAR>, std::basic_string<TCHAR>> expected = {
      {_T("k"),  _T("")},
      {_T("k2"), _T("")},
  };
  BOOST_CHECK(result.parsed_map == expected);
}

BOOST_AUTO_TEST_CASE(short_key_value_no_value_3) {
  const TCHAR *argv[] = {_T("exe_path"), _T("-k"), _T("--k2")};
  auto result = cmdline_options::parse(3, argv);
  std::map<std::basic_string<TCHAR>, std::basic_string<TCHAR>> expected = {
      {_T("k"),  _T("")},
      {_T("k2"), _T("")},
  };
  BOOST_CHECK(result.parsed_map == expected);
}

BOOST_AUTO_TEST_CASE(short_key_value_no_value_4) {
  const TCHAR *argv[] = {_T("exe_path"), _T("-k"), _T("--k2=v2")};
  auto result = cmdline_options::parse(3, argv);
  std::map<std::basic_string<TCHAR>, std::basic_string<TCHAR>> expected = {
      {_T("k"),  _T("")  },
      {_T("k2"), _T("v2")},
  };
  BOOST_CHECK(result.parsed_map == expected);
}

BOOST_AUTO_TEST_CASE(separeted) {
  const TCHAR *argv[] = {_T("exe_path"), _T("k"), _T("v")};
  auto result = cmdline_options::parse(3, argv);
  std::map<std::basic_string<TCHAR>, std::basic_string<TCHAR>> expected = {
      {_T("k"), _T("")},
      {_T("v"), _T("")},
  };
  BOOST_CHECK(result.parsed_map == expected);
}

BOOST_AUTO_TEST_CASE(mixed) {
  const TCHAR *argv[] = {_T("exe_path"), _T("--k1=v1"), _T("-k2"), _T("v2"), _T("k3"), _T("v3")};
  auto result = cmdline_options::parse(6, argv);
  std::map<std::basic_string<TCHAR>, std::basic_string<TCHAR>> expected = {
      {_T("k1"), _T("v1")},
      {_T("k2"), _T("v2")},
      {_T("k3"), _T("")  },
      {_T("v3"), _T("")  },
  };
  BOOST_CHECK(result.parsed_map == expected);
}

BOOST_AUTO_TEST_CASE(mixed_no_order) {
  const TCHAR *argv[] = {_T("exe_path"), _T("--k1=v1"), _T("k3"), _T("-k2"), _T("v2"), _T("v3")};
  auto result = cmdline_options::parse(6, argv);
  std::map<std::basic_string<TCHAR>, std::basic_string<TCHAR>> expected = {
      {_T("k1"), _T("v1")},
      {_T("k2"), _T("v2")},
      {_T("k3"), _T("")  },
      {_T("v3"), _T("")  },
  };
  BOOST_CHECK(result.parsed_map == expected);
}

BOOST_AUTO_TEST_CASE(mixed_no_order_no_value) {
  const TCHAR *argv[] = {_T("exe_path"), _T("--k1=v1"), _T("-k2"), _T("-k3"), _T("v3")};
  auto result = cmdline_options::parse(5, argv);
  std::map<std::basic_string<TCHAR>, std::basic_string<TCHAR>> expected = {
      {_T("k1"), _T("v1")},
      {_T("k2"), _T("")  },
      {_T("k3"), _T("v3")},
  };
  BOOST_CHECK(result.parsed_map == expected);
}

BOOST_AUTO_TEST_CASE(case_translate) {
  const TCHAR *argv[] = {_T("exe_path"), _T("--k1=v1"), _T("-k2"), _T("2"), _T("k3")};
  auto result = cmdline_options::parse(5, argv);
  BOOST_CHECK(result.has(_T("k1")));
  BOOST_CHECK(result.has(_T("k2")));
  BOOST_CHECK(result.has(_T("k3")));
  BOOST_CHECK(!result.has(_T("k4")));
  BOOST_CHECK(result.get(_T("k1")) == _T("v1"));
  BOOST_CHECK(result.get(_T("k2")) == _T("2"));
  BOOST_CHECK(result.get(_T("k3")) == _T(""));
  BOOST_CHECK(result.get(_T("k4")) == _T(""));
  BOOST_CHECK(result.get_as<int>(_T("k1")) == 0);
  BOOST_CHECK(result.get_as<int>(_T("k2")) == 2);
  BOOST_CHECK(result.get_as<int>(_T("k3")) == 0);
  BOOST_CHECK(result.get_as<int>(_T("k4")) == 0);
}

BOOST_AUTO_TEST_SUITE_END()
