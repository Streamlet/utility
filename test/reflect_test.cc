#include <gtest/gtest.h>
#include <xl/reflect>

namespace {

XL_REFLECT_BEGIN(Foo)
  XL_REFLECT_MEMBER(int, a)
  XL_REFLECT_MEMBER(std::string, b)
XL_REFLECT_END()

} // namespace

TEST(reflect_test, normal) {
  ASSERT_EQ(Foo::FIELD_COUNT, 2);

  ASSERT_EQ(strcmp(Foo::Field<0>::name(), "a"), 0);
  ASSERT_EQ(strcmp(Foo::Field<1>::name(), "b"), 0);

  Foo foo;
  foo.a = 123;
  foo.b = "abc";

  ASSERT_EQ(Foo::Field<0>::value(foo), 123);
  ASSERT_EQ(Foo::Field<1>::value(foo), "abc");

  Foo::Field<0>::value(foo) = 456;
  Foo::Field<1>::value(foo) = "def";

  ASSERT_EQ(Foo::Field<0>::value(foo), 456);
  ASSERT_EQ(Foo::Field<1>::value(foo), "def");
}
