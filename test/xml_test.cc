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
#include <xl/string>
#include <xl/xml>

namespace {

const char *EMPTY_XML = R"(<root />)";

std::string remove_blanks(const std::string &s) {
  return xl::string::replace(xl::string::replace(s, "\t", ""), "\n", "");
}

XL_XML_BEGIN(SingleValues)
  XL_XML_MEMBER_ATTR(std::string, attr1)
  XL_XML_MEMBER_ATTR(std::string, attr2)
  XL_XML_MEMBER_NODE(std::string, child1)
  XL_XML_MEMBER_NODE(std::vector<std::string>, child2)
XL_XML_END()

const char *SILNGLE_VALUES_XML = R"(<root attr1="value1" attr2="value2">
	<child1>child1</child1>
	<child2>child2_1</child2>
	<child2>child2_2</child2>
</root>

)";

} // namespace

TEST(xml_test, single_values) {
  SingleValues xml;
  ASSERT_EQ(xml.xml_parse(SILNGLE_VALUES_XML, "root"), true);
  ASSERT_EQ(xml.attr1, "value1");
  ASSERT_EQ(xml.attr2, "value2");
  ASSERT_EQ(xml.child1, "child1");
  ASSERT_EQ(xml.child2, (std::vector<std::string>{"child2_1", "child2_2"}));
  ASSERT_EQ(xml.xml_dump("root", ::xl::xml::WRITE_FLAG_PRETTY), SILNGLE_VALUES_XML);
  ASSERT_EQ(xml.xml_dump("root"), remove_blanks(SILNGLE_VALUES_XML));
}
