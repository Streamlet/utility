// MIT License
//
// Copyright (c) 2022 Streamlet (streamlet@outlook.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <gtest/gtest.h>
#include <xl/scope_exit>

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
