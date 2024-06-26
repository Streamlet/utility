#include "string.h"
#include <gtest/gtest.h>

TEST(string_test, length) {
  ASSERT_EQ(xl::string::length(""), 0);
  ASSERT_EQ(xl::string::length("abc"), 3);
  ASSERT_EQ(xl::string::length("1234567890"), 10);

  ASSERT_EQ(xl::string::length(L""), 0);
  ASSERT_EQ(xl::string::length(L"abc"), 3);
  ASSERT_EQ(xl::string::length(L"1234567890"), 10);
}

#define STRING_REF_LIST(...) (std::vector<xl::string_ref>{__VA_ARGS__})
TEST(string_test, split_ref) {
  ASSERT_EQ(xl::string::split_ref("", ""), STRING_REF_LIST(""));
  ASSERT_EQ(xl::string::split_ref("a", ""), STRING_REF_LIST("a"));
  ASSERT_EQ(xl::string::split_ref("", "a"), STRING_REF_LIST(""));
  ASSERT_EQ(xl::string::split_ref("a", "a"), STRING_REF_LIST("", ""));
  ASSERT_EQ(xl::string::split_ref("aa", "a"), STRING_REF_LIST("", "", ""));
  ASSERT_EQ(xl::string::split_ref("aba", "a"), STRING_REF_LIST("", "b", ""));
  ASSERT_EQ(xl::string::split_ref("bab", "a"), STRING_REF_LIST("b", "b"));
  ASSERT_EQ(xl::string::split_ref("abc|def|g", "|"), STRING_REF_LIST("abc", "def", "g"));
  ASSERT_EQ(xl::string::split_ref("abc|def|g", "|", 1), STRING_REF_LIST("abc|def|g"));
  ASSERT_EQ(xl::string::split_ref("abc|def|g", "|", 2), STRING_REF_LIST("abc", "def|g"));
  ASSERT_EQ(xl::string::split_ref("abc|def|g", "|", 3), STRING_REF_LIST("abc", "def", "g"));
  ASSERT_EQ(xl::string::split_ref("abc|def|g", "|", 4), STRING_REF_LIST("abc", "def", "g"));
  ASSERT_EQ(xl::string::split_ref("abc|def|g", "|", 5), STRING_REF_LIST("abc", "def", "g"));
}

#define WSTRING_REF_LIST(...) (std::vector<xl::wstring_ref>{__VA_ARGS__})
TEST(string_test, split_ref_w) {
  ASSERT_EQ(xl::string::split_ref(L"", L""), WSTRING_REF_LIST(L""));
  ASSERT_EQ(xl::string::split_ref(L"a", L""), WSTRING_REF_LIST(L"a"));
  ASSERT_EQ(xl::string::split_ref(L"", L"a"), WSTRING_REF_LIST(L""));
  ASSERT_EQ(xl::string::split_ref(L"a", L"a"), WSTRING_REF_LIST(L"", L""));
  ASSERT_EQ(xl::string::split_ref(L"aa", L"a"), WSTRING_REF_LIST(L"", L"", L""));
  ASSERT_EQ(xl::string::split_ref(L"aba", L"a"), WSTRING_REF_LIST(L"", L"b", L""));
  ASSERT_EQ(xl::string::split_ref(L"bab", L"a"), WSTRING_REF_LIST(L"b", L"b"));
  ASSERT_EQ(xl::string::split_ref(L"abc|def|g", L"|"), WSTRING_REF_LIST(L"abc", L"def", L"g"));
  ASSERT_EQ(xl::string::split_ref(L"abc|def|g", L"|", 1), WSTRING_REF_LIST(L"abc|def|g"));
  ASSERT_EQ(xl::string::split_ref(L"abc|def|g", L"|", 2), WSTRING_REF_LIST(L"abc", L"def|g"));
  ASSERT_EQ(xl::string::split_ref(L"abc|def|g", L"|", 3), WSTRING_REF_LIST(L"abc", L"def", L"g"));
  ASSERT_EQ(xl::string::split_ref(L"abc|def|g", L"|", 4), WSTRING_REF_LIST(L"abc", L"def", L"g"));
  ASSERT_EQ(xl::string::split_ref(L"abc|def|g", L"|", 5), WSTRING_REF_LIST(L"abc", L"def", L"g"));
}

#define STRING_LIST(...) (std::vector<std::string>{__VA_ARGS__})
TEST(string_test, split) {
  ASSERT_EQ(xl::string::split("", ""), STRING_LIST(""));
  ASSERT_EQ(xl::string::split("a", ""), STRING_LIST("a"));
  ASSERT_EQ(xl::string::split("", "a"), STRING_LIST(""));
  ASSERT_EQ(xl::string::split("a", "a"), STRING_LIST("", ""));
  ASSERT_EQ(xl::string::split("aa", "a"), STRING_LIST("", "", ""));
  ASSERT_EQ(xl::string::split("aba", "a"), STRING_LIST("", "b", ""));
  ASSERT_EQ(xl::string::split("bab", "a"), STRING_LIST("b", "b"));
  ASSERT_EQ(xl::string::split("abc|def|g", "|"), STRING_LIST("abc", "def", "g"));
  ASSERT_EQ(xl::string::split("abc|def|g", "|", 1), STRING_LIST("abc|def|g"));
  ASSERT_EQ(xl::string::split("abc|def|g", "|", 2), STRING_LIST("abc", "def|g"));
  ASSERT_EQ(xl::string::split("abc|def|g", "|", 3), STRING_LIST("abc", "def", "g"));
  ASSERT_EQ(xl::string::split("abc|def|g", "|", 4), STRING_LIST("abc", "def", "g"));
  ASSERT_EQ(xl::string::split("abc|def|g", "|", 5), STRING_LIST("abc", "def", "g"));
}

#define WSTRING_LIST(...) (std::vector<std::wstring>{__VA_ARGS__})
TEST(string_test, split_w) {
  ASSERT_EQ(xl::string::split(L"", L""), WSTRING_LIST(L""));
  ASSERT_EQ(xl::string::split(L"a", L""), WSTRING_LIST(L"a"));
  ASSERT_EQ(xl::string::split(L"", L"a"), WSTRING_LIST(L""));
  ASSERT_EQ(xl::string::split(L"a", L"a"), WSTRING_LIST(L"", L""));
  ASSERT_EQ(xl::string::split(L"aa", L"a"), WSTRING_LIST(L"", L"", L""));
  ASSERT_EQ(xl::string::split(L"aba", L"a"), WSTRING_LIST(L"", L"b", L""));
  ASSERT_EQ(xl::string::split(L"bab", L"a"), WSTRING_LIST(L"b", L"b"));
  ASSERT_EQ(xl::string::split(L"abc|def|g", L"|"), WSTRING_LIST(L"abc", L"def", L"g"));
  ASSERT_EQ(xl::string::split(L"abc|def|g", L"|", 1), WSTRING_LIST(L"abc|def|g"));
  ASSERT_EQ(xl::string::split(L"abc|def|g", L"|", 2), WSTRING_LIST(L"abc", L"def|g"));
  ASSERT_EQ(xl::string::split(L"abc|def|g", L"|", 3), WSTRING_LIST(L"abc", L"def", L"g"));
  ASSERT_EQ(xl::string::split(L"abc|def|g", L"|", 4), WSTRING_LIST(L"abc", L"def", L"g"));
  ASSERT_EQ(xl::string::split(L"abc|def|g", L"|", 5), WSTRING_LIST(L"abc", L"def", L"g"));
}

TEST(string_test, join) {
  ASSERT_EQ(xl::string::join(STRING_LIST(""), ""), "");
  ASSERT_EQ(xl::string::join(STRING_LIST(""), "|"), "");
  ASSERT_EQ(xl::string::join(STRING_LIST("", ""), ""), "");
  ASSERT_EQ(xl::string::join(STRING_LIST("", ""), "|"), "|");
  ASSERT_EQ(xl::string::join(STRING_LIST("", "", ""), ""), "");
  ASSERT_EQ(xl::string::join(STRING_LIST("", "", ""), "|"), "||");
  ASSERT_EQ(xl::string::join(STRING_LIST("a"), ""), "a");
  ASSERT_EQ(xl::string::join(STRING_LIST("a"), "|"), "a");
  ASSERT_EQ(xl::string::join(STRING_LIST("a", "b"), ""), "ab");
  ASSERT_EQ(xl::string::join(STRING_LIST("a", "b"), "|"), "a|b");
  ASSERT_EQ(xl::string::join(STRING_LIST("a", "b", "c"), ""), "abc");
  ASSERT_EQ(xl::string::join(STRING_LIST("a", "b", "c"), "|"), "a|b|c");
  ASSERT_EQ(xl::string::join(STRING_LIST("aa"), ""), "aa");
  ASSERT_EQ(xl::string::join(STRING_LIST("aa"), "|"), "aa");
  ASSERT_EQ(xl::string::join(STRING_LIST("aa", "bb"), ""), "aabb");
  ASSERT_EQ(xl::string::join(STRING_LIST("aa", "bb"), "|"), "aa|bb");
  ASSERT_EQ(xl::string::join(STRING_LIST("aa", "bb", "cc"), ""), "aabbcc");
  ASSERT_EQ(xl::string::join(STRING_LIST("aa", "bb", "cc"), "|"), "aa|bb|cc");

  ASSERT_EQ(xl::string::join(STRING_REF_LIST(""), ""), "");
  ASSERT_EQ(xl::string::join(STRING_REF_LIST(""), "|"), "");
  ASSERT_EQ(xl::string::join(STRING_REF_LIST("", ""), ""), "");
  ASSERT_EQ(xl::string::join(STRING_REF_LIST("", ""), "|"), "|");
  ASSERT_EQ(xl::string::join(STRING_REF_LIST("", "", ""), ""), "");
  ASSERT_EQ(xl::string::join(STRING_REF_LIST("", "", ""), "|"), "||");
  ASSERT_EQ(xl::string::join(STRING_REF_LIST("a"), ""), "a");
  ASSERT_EQ(xl::string::join(STRING_REF_LIST("a"), "|"), "a");
  ASSERT_EQ(xl::string::join(STRING_REF_LIST("a", "b"), ""), "ab");
  ASSERT_EQ(xl::string::join(STRING_REF_LIST("a", "b"), "|"), "a|b");
  ASSERT_EQ(xl::string::join(STRING_REF_LIST("a", "b", "c"), ""), "abc");
  ASSERT_EQ(xl::string::join(STRING_REF_LIST("a", "b", "c"), "|"), "a|b|c");
  ASSERT_EQ(xl::string::join(STRING_REF_LIST("aa"), ""), "aa");
  ASSERT_EQ(xl::string::join(STRING_REF_LIST("aa"), "|"), "aa");
  ASSERT_EQ(xl::string::join(STRING_REF_LIST("aa", "bb"), ""), "aabb");
  ASSERT_EQ(xl::string::join(STRING_REF_LIST("aa", "bb"), "|"), "aa|bb");
  ASSERT_EQ(xl::string::join(STRING_REF_LIST("aa", "bb", "cc"), ""), "aabbcc");
  ASSERT_EQ(xl::string::join(STRING_REF_LIST("aa", "bb", "cc"), "|"), "aa|bb|cc");
}

TEST(string_test, join_w) {
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L""), L""), L"");
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L""), L"|"), L"");
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L"", L""), L""), L"");
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L"", L""), L"|"), L"|");
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L"", L"", L""), L""), L"");
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L"", L"", L""), L"|"), L"||");
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L"a"), L""), L"a");
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L"a"), L"|"), L"a");
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L"a", L"b"), L""), L"ab");
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L"a", L"b"), L"|"), L"a|b");
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L"a", L"b", L"c"), L""), L"abc");
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L"a", L"b", L"c"), L"|"), L"a|b|c");
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L"aa"), L""), L"aa");
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L"aa"), L"|"), L"aa");
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L"aa", L"bb"), L""), L"aabb");
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L"aa", L"bb"), L"|"), L"aa|bb");
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L"aa", L"bb", L"cc"), L""), L"aabbcc");
  ASSERT_EQ(xl::string::join(WSTRING_LIST(L"aa", L"bb", L"cc"), L"|"), L"aa|bb|cc");

  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L""), L""), L"");
  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L""), L"|"), L"");
  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L"", L""), L""), L"");
  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L"", L""), L"|"), L"|");
  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L"", L"", L""), L""), L"");
  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L"", L"", L""), L"|"), L"||");
  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L"a"), L""), L"a");
  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L"a"), L"|"), L"a");
  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L"a", L"b"), L""), L"ab");
  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L"a", L"b"), L"|"), L"a|b");
  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L"a", L"b", L"c"), L""), L"abc");
  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L"a", L"b", L"c"), L"|"), L"a|b|c");
  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L"aa"), L""), L"aa");
  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L"aa"), L"|"), L"aa");
  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L"aa", L"bb"), L""), L"aabb");
  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L"aa", L"bb"), L"|"), L"aa|bb");
  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L"aa", L"bb", L"cc"), L""), L"aabbcc");
  ASSERT_EQ(xl::string::join(WSTRING_REF_LIST(L"aa", L"bb", L"cc"), L"|"), L"aa|bb|cc");
}

TEST(string_test, replace) {
  ASSERT_EQ(xl::string::replace("", "", ""), "");
  ASSERT_EQ(xl::string::replace("", "", "x"), "");
  ASSERT_EQ(xl::string::replace("", "a", "x"), "");
  ASSERT_EQ(xl::string::replace("", "aa", "x"), "");
  ASSERT_EQ(xl::string::replace("", "a", "xx"), "");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "", ""), "abcaabbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "", "x"), "abcaabbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "a", "x"), "xbcxxbbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "aa", "x"), "abcxbbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "a", "xy"), "xybcxyxybbcc");

  ASSERT_EQ(xl::string::replace("abcaabbcc", "", "", 1), "abcaabbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "", "x", 1), "abcaabbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "a", "x", 1), "xbcaabbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "aa", "x", 1), "abcxbbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "a", "xy", 1), "xybcaabbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "", "", 2), "abcaabbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "", "x", 2), "abcaabbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "a", "x", 2), "xbcxabbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "aa", "x", 2), "abcxbbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "a", "xy", 2), "xybcxyabbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "", "", 3), "abcaabbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "", "x", 3), "abcaabbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "a", "x", 3), "xbcxxbbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "aa", "x", 3), "abcxbbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "a", "xy", 3), "xybcxyxybbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "", "", 4), "abcaabbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "", "x", 4), "abcaabbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "a", "x", 4), "xbcxxbbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "aa", "x", 4), "abcxbbcc");
  ASSERT_EQ(xl::string::replace("abcaabbcc", "a", "xy", 4), "xybcxyxybbcc");
}

TEST(string_test, replace_w) {
  ASSERT_EQ(xl::string::replace(L"", L"", L""), L"");
  ASSERT_EQ(xl::string::replace(L"", L"", L"x"), L"");
  ASSERT_EQ(xl::string::replace(L"", L"a", L"x"), L"");
  ASSERT_EQ(xl::string::replace(L"", L"aa", L"x"), L"");
  ASSERT_EQ(xl::string::replace(L"", L"a", L"xx"), L"");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"", L""), L"abcaabbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"", L"x"), L"abcaabbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"a", L"x"), L"xbcxxbbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"aa", L"x"), L"abcxbbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"a", L"xy"), L"xybcxyxybbcc");

  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"", L"", 1), L"abcaabbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"", L"x", 1), L"abcaabbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"a", L"x", 1), L"xbcaabbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"aa", L"x", 1), L"abcxbbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"a", L"xy", 1), L"xybcaabbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"", L"", 2), L"abcaabbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"", L"x", 2), L"abcaabbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"a", L"x", 2), L"xbcxabbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"aa", L"x", 2), L"abcxbbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"a", L"xy", 2), L"xybcxyabbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"", L"", 3), L"abcaabbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"", L"x", 3), L"abcaabbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"a", L"x", 3), L"xbcxxbbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"aa", L"x", 3), L"abcxbbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"a", L"xy", 3), L"xybcxyxybbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"", L"", 4), L"abcaabbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"", L"x", 4), L"abcaabbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"a", L"x", 4), L"xbcxxbbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"aa", L"x", 4), L"abcxbbcc");
  ASSERT_EQ(xl::string::replace(L"abcaabbcc", L"a", L"xy", 4), L"xybcxyxybbcc");
}
