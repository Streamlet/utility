#include "encoding.h"
#include <gtest/gtest.h>

TEST(encoding_test, utf8_to_utf16) {
  ASSERT_EQ(xl::encoding::utf8_to_utf16(u8"你好"), L"你好");
  ASSERT_EQ(xl::encoding::utf8_to_utf16(u8"𐐷𪺫"), L"𐐷𪺫"); // U+10437, U+2AEAB
}

TEST(encoding_test, utf16_to_utf8) {
  ASSERT_EQ(xl::encoding::utf16_to_utf8(L"你好"), u8"你好");
  ASSERT_EQ(xl::encoding::utf16_to_utf8(L"𐐷𪺫"), u8"𐐷𪺫"); // U+10437, U+2AEAB
}
