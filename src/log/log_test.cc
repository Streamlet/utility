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
#include <xl/file>
#include <xl/log>
#include <xl/log_setup>

TEST(log_test, normal) {
  xl::fs::unlink(_T("test.log"));
  xl::log::setup(_T("test"), XL_LOG_LEVEL_INFO, xl::log::LOG_CONTENT_LEVEL | xl::log::LOG_CONTENT_APP_NAME,
                 xl::log::LOG_TARGET_FILE, _T("test.log"));

  XL_LOG_FATAL("fatal ", 1, " 2 ", 3, " log");
  XL_LOG_ERROR("error ", 1, " 2 ", 3, " log");
  XL_LOG_WARN("warn ", 1, " 2 ", 3, " log");
  XL_LOG_INFO("info ", 1, " 2 ", 3, " log");
  XL_LOG_DEBUG("debug ", 1, " 2 ", 3, " log");

  xl::log::shutdown();

  ASSERT_EQ(xl::file::read(_T("test.log")), "[FATAL][test]fatal 1 2 3 log\n"
                                            "[ERROR][test]error 1 2 3 log\n"
                                            "[WARN][test]warn 1 2 3 log\n"
                                            "[INFO][test]info 1 2 3 log\n");

  ASSERT_EQ(xl::fs::unlink(_T("test.log")), true);
}
