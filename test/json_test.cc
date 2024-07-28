#include <gtest/gtest.h>
#include <xl/json>

XL_JSON_BEGIN(Foo)
  XL_JSON_MEMBER(int, a)
  XL_JSON_MEMBER(std::string, b)
  XL_JSON_MEMBER(std::unique_ptr<std::string>, c)
XL_JSON_END()

TEST(json_test, normal) {
  Foo foo;
  ASSERT_EQ(foo.parse(R"({"a":1,"b":"x","c":null})"), true);
  ASSERT_EQ(foo.a, 1);
  ASSERT_EQ(foo.b, "x");
  ASSERT_EQ(foo.c, nullptr);
  std::string json = foo.dump();
  ASSERT_EQ(json, R"({"a":1,"b":"x","c":null})");
}
