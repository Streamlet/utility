#include <gtest/gtest.h>
#include <xl/scope_exit.h>

void function_set(int *variable, int value) {
  *variable = value;
}

class functor_set {
public:
  void operator()(int *variable, int value) {
    *variable = value;
  }
};

class member_set {
public:
  void set(int *variable, int value) {
    *variable = value;
  }
};

auto lambda_set = [](int *variable, int value) {
  *variable = value;
};

TEST(scope_exit_test, normal) {
  int i = 0;
  ASSERT_EQ(i, 0);
  function_set(&i, 1);
  ASSERT_EQ(i, 1);
  functor_set fs;
  fs(&i, 2);
  ASSERT_EQ(i, 2);
  member_set ms;
  ms.set(&i, 3);
  ASSERT_EQ(i, 3);
  lambda_set(&i, 4);
  ASSERT_EQ(i, 4);

  i = 0;
  ASSERT_EQ(i, 0);
  {
    i = -1;
    ASSERT_EQ(i, -1);
    XL_ON_BLOCK_EXIT(function_set, &i, 1);
  }
  ASSERT_EQ(i, 1);
  {
    i = -1;
    ASSERT_EQ(i, -1);
    XL_ON_BLOCK_EXIT(fs, &i, 2);
  }
  ASSERT_EQ(i, 2);
  {
    i = -1;
    ASSERT_EQ(i, -1);
    XL_ON_BLOCK_EXIT(&member_set::set, &ms, &i, 3);
  }
  ASSERT_EQ(i, 3);
  {
    i = -1;
    ASSERT_EQ(i, -1);
    XL_ON_BLOCK_EXIT(lambda_set, &i, 4);
  }
  ASSERT_EQ(i, 4);
}
