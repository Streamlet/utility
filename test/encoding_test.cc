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
#include <xl/encoding>

TEST(encoding_test, utf8_to_utf16) {
  ASSERT_EQ(xl::encoding::utf8_to_utf16("你好"), L"你好");
  ASSERT_EQ(xl::encoding::utf8_to_utf16("𐐷𪺫"), L"\xD801\xDC37\xD86B\xDEAB"); // U+10437, U+2AEAB
}

TEST(encoding_test, utf16_to_utf8) {
  ASSERT_EQ(xl::encoding::utf16_to_utf8(L"你好"), "你好");
  ASSERT_EQ(xl::encoding::utf16_to_utf8(L"𐐷𪺫"), "𐐷𪺫"); // U+10437, U+2AEAB
}
