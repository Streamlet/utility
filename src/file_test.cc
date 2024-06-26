#include "file.h"
#include <gtest/gtest.h>

TEST(file_test, fs_operation) {
  ASSERT_EQ(xl::file::exists(_T("f")), false);
  ASSERT_EQ(xl::file::size(_T("f")), -1);
  ASSERT_EQ(xl::file::touch(_T("f")), true);
  ASSERT_EQ(xl::file::exists(_T("f")), true);
  ASSERT_EQ(xl::file::size(_T("f")), 0);
  ASSERT_EQ(xl::file::rename(_T("f"), _T("f1")), true);
  ASSERT_EQ(xl::file::exists(_T("f")), false);
  ASSERT_EQ(xl::file::exists(_T("f1")), true);
  ASSERT_EQ(xl::file::size(_T("f1")), 0);
  ASSERT_EQ(xl::file::remove(_T("f1")), true);
  ASSERT_EQ(xl::file::exists(_T("f")), false);
  ASSERT_EQ(xl::file::exists(_T("f1")), false);
}

TEST(file_test, bin_and_utf8) {
  ASSERT_EQ(xl::file::write(_T("f"), "\xef\xbb\xbf"
                                     "你好"),
            true);
  ASSERT_EQ(xl::file::read(_T("f")), "\xef\xbb\xbf"
                                     "你好");
  ASSERT_EQ(xl::file::read_text_utf8_bom(_T("f")), "你好");
  ASSERT_EQ(xl::file::read_text_utf16_le(_T("f")), L"");
  ASSERT_EQ(xl::file::read_text_utf16_be(_T("f")), L"");
  ASSERT_EQ(xl::file::read_text_auto(_T("f")), "你好");

  ASSERT_EQ(xl::file::write(_T("f"), "你好"), true);
  ASSERT_EQ(xl::file::read(_T("f")), "你好");
  ASSERT_EQ(xl::file::read_text_utf8_bom(_T("f")), "");
  ASSERT_EQ(xl::file::read_text_utf16_le(_T("f")), L"");
  ASSERT_EQ(xl::file::read_text_utf16_be(_T("f")), L"");
  ASSERT_EQ(xl::file::read_text_auto(_T("f")), "你好");

  ASSERT_EQ(xl::file::remove(_T("f")), true);
}

TEST(file_test, utf16_le) {

  ASSERT_EQ(xl::file::write_text_utf16_le(_T("f"), L"你好"), true); // U+4F60 U+597D
  ASSERT_EQ(xl::file::read(_T("f")), "\xff\xfe\x60\x4f\x7d\x59");
  ASSERT_EQ(xl::file::read_text_utf8_bom(_T("f")), "");
  ASSERT_EQ(xl::file::read_text_utf16_le(_T("f")), L"你好");
  ASSERT_EQ(xl::file::read_text_utf16_be(_T("f")), L"");
  ASSERT_EQ(xl::file::read_text_auto(_T("f")), "你好");
  ASSERT_EQ(xl::file::remove(_T("f")), true);
}

TEST(file_test, utf16_be) {
  ASSERT_EQ(xl::file::write_text_utf16_be(_T("f"), L"你好"), true); // U+4F60 U+597D
  ASSERT_EQ(xl::file::read(_T("f")), "\xfe\xff\x4f\x60\x59\x7d");
  ASSERT_EQ(xl::file::read_text_utf8_bom(_T("f")), "");
  ASSERT_EQ(xl::file::read_text_utf16_le(_T("f")), L"");
  ASSERT_EQ(xl::file::read_text_utf16_be(_T("f")), L"你好");
  ASSERT_EQ(xl::file::read_text_auto(_T("f")), "你好");
  ASSERT_EQ(xl::file::remove(_T("f")), true);
}
