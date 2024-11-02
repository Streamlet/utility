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
#include <xl/process>
#include <xl/task_thread>

TEST(task_thread_test, normal) {
  int tid = xl::process::tid();
  xl::task_thread tt;
  int task_tid1 = 0;
  tt.post_task([&]() {
    task_tid1 = xl::process::tid();
    ASSERT_NE(task_tid1, tid);
  });
  int task_tid2 = 0;
  tt.post_task([&]() {
    task_tid2 = xl::process::tid();
    ASSERT_EQ(task_tid2, task_tid1);
  });
  while (task_tid2 == 0) {
    xl::process::sleep(100);
  }
  tt.quit();
  tt.join();
}
