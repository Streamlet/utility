#include <gtest/gtest.h>
#include <xl/reflect>

namespace {

#pragma pack(push, 1)

XL_REFLECT_BEGIN(Foo)
  XL_REFLECT_MEMBER(int, a)
  XL_REFLECT_MEMBER(const char *, b)
XL_REFLECT_END()

#pragma pack(pop)

} // namespace

TEST(reflect_test, normal) {
  ASSERT_EQ(Foo::fields(), 2);

  ASSERT_EQ(Foo::Field<0>::type(), typeid(int));
  ASSERT_EQ(Foo::Field<1>::type(), typeid(const char *));
  ASSERT_EQ(strcmp(Foo::Field<0>::name(), "a"), 0);
  ASSERT_EQ(strcmp(Foo::Field<1>::name(), "b"), 0);
  ASSERT_EQ(Foo::Field<0>::offset(), 0);
  ASSERT_EQ(Foo::Field<1>::offset(), sizeof(int));
  ASSERT_EQ(Foo::Field<0>::size(), sizeof(int));
  ASSERT_EQ(Foo::Field<1>::size(), sizeof(const char *));

  ASSERT_EQ(Foo::field_type(0), typeid(int));
  ASSERT_EQ(Foo::field_type(1), typeid(const char *));
  ASSERT_EQ(strcmp(Foo::field_name(0), "a"), 0);
  ASSERT_EQ(strcmp(Foo::field_name(1), "b"), 0);
  ASSERT_EQ(Foo::field_index("a"), 0);
  ASSERT_EQ(Foo::field_index("b"), 1);
  ASSERT_EQ(Foo::field_index("aa", 1), 0);
  ASSERT_EQ(Foo::field_index("bb", 1), 1);
  ASSERT_EQ(Foo::field_index("aa", 2), -1);
  ASSERT_EQ(Foo::field_index("bb", 2), -1);
  ASSERT_EQ(Foo::field_offset(0), 0);
  ASSERT_EQ(Foo::field_offset(1), sizeof(int));
  ASSERT_EQ(Foo::field_size(0), sizeof(int));
  ASSERT_EQ(Foo::field_size(1), sizeof(const char *));

  Foo foo;
  foo.a = 123;
  foo.b = "abc";

  ASSERT_EQ(Foo::Field<0>::value(foo), 123);
  ASSERT_EQ(strcmp(Foo::Field<1>::value(foo), "abc"), 0);
  ASSERT_EQ(*(int *)foo.field_data(0), 123);
  ASSERT_EQ(strcmp(*(const char **)foo.field_data(1), "abc"), 0);

  Foo::Field<0>::value(foo) = 456;
  Foo::Field<1>::value(foo) = "def";

  ASSERT_EQ(Foo::Field<0>::value(foo), 456);
  ASSERT_EQ(strcmp(Foo::Field<1>::value(foo), "def"), 0);
  ASSERT_EQ(*(int *)foo.field_data(0), 456);
  ASSERT_EQ(strcmp(*(const char **)foo.field_data(1), "def"), 0);

  foo.field_value<int>(0) = 789;
  foo.field_value<const char *>(1) = "ghi";
  ASSERT_EQ(*(int *)foo.field_data(0), 789);
  ASSERT_EQ(strcmp(*(const char **)foo.field_data(1), "ghi"), 0);
}
