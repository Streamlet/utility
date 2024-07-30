#include <gtest/gtest.h>
#include <xl/json>
#include <xl/string>

const char *EMPTY_JSON = R"({})";

namespace {

std::string remove_blanks(const std::string &s) {
  return xl::string::replace(xl::string::replace(s, " ", ""), "\n", "");
}

} // namespace

XL_JSON_BEGIN(SingleValues)
  XL_JSON_MEMBER(bool, boolValue)
  XL_JSON_MEMBER(char, charValue)
  XL_JSON_MEMBER(unsigned char, ucharValue)
  XL_JSON_MEMBER(short, shortValue)
  XL_JSON_MEMBER(unsigned short, ushortValue)
  XL_JSON_MEMBER(int, intValue)
  XL_JSON_MEMBER(unsigned int, uintValue)
  XL_JSON_MEMBER(long, longValue)
  XL_JSON_MEMBER(unsigned long, ulongValue)
  XL_JSON_MEMBER(long long, llongValue)
  XL_JSON_MEMBER(unsigned long long, ullongValue)
  XL_JSON_MEMBER(float, floatValue)
  XL_JSON_MEMBER(double, doubleValue)
  XL_JSON_MEMBER(std::string, stringValue)
XL_JSON_END()

const char *SILNGLE_VALUES_JSON = R"({
    "boolValue": true,
    "charValue": -1,
    "ucharValue": 2,
    "shortValue": -3,
    "ushortValue": 4,
    "intValue": -5,
    "uintValue": 6,
    "longValue": -7,
    "ulongValue": 8,
    "llongValue": -9,
    "ullongValue": 10,
    "floatValue": 1.5,
    "doubleValue": 2.25,
    "stringValue": "s"
})";

TEST(json_test, single_values) {
  SingleValues json;
  ASSERT_EQ(json.json_parse(SILNGLE_VALUES_JSON), true);
  ASSERT_EQ(json.boolValue, true);
  ASSERT_EQ(json.charValue, -1);
  ASSERT_EQ(json.ucharValue, 2);
  ASSERT_EQ(json.shortValue, -3);
  ASSERT_EQ(json.ushortValue, 4);
  ASSERT_EQ(json.intValue, -5);
  ASSERT_EQ(json.uintValue, 6);
  ASSERT_EQ(json.longValue, -7);
  ASSERT_EQ(json.ulongValue, 8);
  ASSERT_EQ(json.llongValue, -9);
  ASSERT_EQ(json.ullongValue, 10);
  ASSERT_EQ(json.floatValue, 1.5);
  ASSERT_EQ(json.doubleValue, 2.25);
  ASSERT_EQ(json.stringValue, "s");
  ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), SILNGLE_VALUES_JSON);
  ASSERT_EQ(json.json_dump(), remove_blanks(SILNGLE_VALUES_JSON));
}

XL_JSON_BEGIN(NullableValues)
  XL_JSON_MEMBER(std::unique_ptr<bool>, boolValue)
  XL_JSON_MEMBER(std::unique_ptr<char>, charValue)
  XL_JSON_MEMBER(std::unique_ptr<unsigned char>, ucharValue)
  XL_JSON_MEMBER(std::unique_ptr<short>, shortValue)
  XL_JSON_MEMBER(std::unique_ptr<unsigned short>, ushortValue)
  XL_JSON_MEMBER(std::unique_ptr<int>, intValue)
  XL_JSON_MEMBER(std::unique_ptr<unsigned int>, uintValue)
  XL_JSON_MEMBER(std::unique_ptr<long>, longValue)
  XL_JSON_MEMBER(std::unique_ptr<unsigned long>, ulongValue)
  XL_JSON_MEMBER(std::unique_ptr<long long>, llongValue)
  XL_JSON_MEMBER(std::unique_ptr<unsigned long long>, ullongValue)
  XL_JSON_MEMBER(std::unique_ptr<float>, floatValue)
  XL_JSON_MEMBER(std::unique_ptr<double>, doubleValue)
  XL_JSON_MEMBER(std::unique_ptr<std::string>, stringValue)
XL_JSON_END()

const char *NULL_VALUES_JSON = R"({
    "boolValue": null,
    "charValue": null,
    "ucharValue": null,
    "shortValue": null,
    "ushortValue": null,
    "intValue": null,
    "uintValue": null,
    "longValue": null,
    "ulongValue": null,
    "llongValue": null,
    "ullongValue": null,
    "floatValue": null,
    "doubleValue": null,
    "stringValue": null
})";

const char *HALF_MISSING_VALUES_JSON = R"({
    "boolValue": true,
    "intValue": -5,
    "uintValue": 6,
    "longValue": -7,
    "ulongValue": 8,
    "llongValue": -9,
    "ullongValue": 10,
    "floatValue": 1.5,
    "doubleValue": 2.25,
    "stringValue": "s"
})";

const char *HALF_NULL_VALUES_JSON = R"({
    "boolValue": true,
    "charValue": null,
    "ucharValue": null,
    "shortValue": null,
    "ushortValue": null,
    "intValue": -5,
    "uintValue": 6,
    "longValue": -7,
    "ulongValue": 8,
    "llongValue": -9,
    "ullongValue": 10,
    "floatValue": 1.5,
    "doubleValue": 2.25,
    "stringValue": "s"
})";

TEST(json_test, nullable_values) {
  {
    NullableValues json;
    ASSERT_EQ(json.json_parse(EMPTY_JSON), true);
    ASSERT_EQ(json.boolValue, nullptr);
    ASSERT_EQ(json.charValue, nullptr);
    ASSERT_EQ(json.ucharValue, nullptr);
    ASSERT_EQ(json.shortValue, nullptr);
    ASSERT_EQ(json.ushortValue, nullptr);
    ASSERT_EQ(json.intValue, nullptr);
    ASSERT_EQ(json.uintValue, nullptr);
    ASSERT_EQ(json.longValue, nullptr);
    ASSERT_EQ(json.ulongValue, nullptr);
    ASSERT_EQ(json.llongValue, nullptr);
    ASSERT_EQ(json.ullongValue, nullptr);
    ASSERT_EQ(json.floatValue, nullptr);
    ASSERT_EQ(json.doubleValue, nullptr);
    ASSERT_EQ(json.stringValue, nullptr);
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), EMPTY_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(EMPTY_JSON));
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY | xl::json::WRITE_FLAG_WRITE_NULL_VALUES), NULL_VALUES_JSON);
    ASSERT_EQ(json.json_dump(xl::json::WRITE_FLAG_WRITE_NULL_VALUES), remove_blanks(NULL_VALUES_JSON));
  }
  {
    NullableValues json;
    ASSERT_EQ(json.json_parse(NULL_VALUES_JSON), true);
    ASSERT_EQ(json.boolValue, nullptr);
    ASSERT_EQ(json.charValue, nullptr);
    ASSERT_EQ(json.ucharValue, nullptr);
    ASSERT_EQ(json.shortValue, nullptr);
    ASSERT_EQ(json.ushortValue, nullptr);
    ASSERT_EQ(json.intValue, nullptr);
    ASSERT_EQ(json.uintValue, nullptr);
    ASSERT_EQ(json.longValue, nullptr);
    ASSERT_EQ(json.ulongValue, nullptr);
    ASSERT_EQ(json.llongValue, nullptr);
    ASSERT_EQ(json.ullongValue, nullptr);
    ASSERT_EQ(json.floatValue, nullptr);
    ASSERT_EQ(json.doubleValue, nullptr);
    ASSERT_EQ(json.stringValue, nullptr);
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), EMPTY_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(EMPTY_JSON));
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY | xl::json::WRITE_FLAG_WRITE_NULL_VALUES), NULL_VALUES_JSON);
    ASSERT_EQ(json.json_dump(xl::json::WRITE_FLAG_WRITE_NULL_VALUES), remove_blanks(NULL_VALUES_JSON));
  }
  {
    NullableValues json;
    ASSERT_EQ(json.json_parse(HALF_MISSING_VALUES_JSON), true);
    ASSERT_EQ(*json.boolValue, true);
    ASSERT_EQ(json.charValue, nullptr);
    ASSERT_EQ(json.ucharValue, nullptr);
    ASSERT_EQ(json.shortValue, nullptr);
    ASSERT_EQ(json.ushortValue, nullptr);
    ASSERT_EQ(*json.intValue, -5);
    ASSERT_EQ(*json.uintValue, 6);
    ASSERT_EQ(*json.longValue, -7);
    ASSERT_EQ(*json.ulongValue, 8);
    ASSERT_EQ(*json.llongValue, -9);
    ASSERT_EQ(*json.ullongValue, 10);
    ASSERT_EQ(*json.floatValue, 1.5);
    ASSERT_EQ(*json.doubleValue, 2.25);
    ASSERT_EQ(*json.stringValue, "s");
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), HALF_MISSING_VALUES_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(HALF_MISSING_VALUES_JSON));
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY | xl::json::WRITE_FLAG_WRITE_NULL_VALUES),
              HALF_NULL_VALUES_JSON);
    ASSERT_EQ(json.json_dump(xl::json::WRITE_FLAG_WRITE_NULL_VALUES), remove_blanks(HALF_NULL_VALUES_JSON));
  }
  {
    NullableValues json;
    ASSERT_EQ(json.json_parse(HALF_NULL_VALUES_JSON), true);
    ASSERT_EQ(*json.boolValue, true);
    ASSERT_EQ(json.charValue, nullptr);
    ASSERT_EQ(json.ucharValue, nullptr);
    ASSERT_EQ(json.shortValue, nullptr);
    ASSERT_EQ(json.ushortValue, nullptr);
    ASSERT_EQ(*json.intValue, -5);
    ASSERT_EQ(*json.uintValue, 6);
    ASSERT_EQ(*json.longValue, -7);
    ASSERT_EQ(*json.ulongValue, 8);
    ASSERT_EQ(*json.llongValue, -9);
    ASSERT_EQ(*json.ullongValue, 10);
    ASSERT_EQ(*json.floatValue, 1.5);
    ASSERT_EQ(*json.doubleValue, 2.25);
    ASSERT_EQ(*json.stringValue, "s");
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), HALF_MISSING_VALUES_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(HALF_MISSING_VALUES_JSON));
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY | xl::json::WRITE_FLAG_WRITE_NULL_VALUES),
              HALF_NULL_VALUES_JSON);
    ASSERT_EQ(json.json_dump(xl::json::WRITE_FLAG_WRITE_NULL_VALUES), remove_blanks(HALF_NULL_VALUES_JSON));
  }
  {
    NullableValues json;
    ASSERT_EQ(json.json_parse(SILNGLE_VALUES_JSON), true);
    ASSERT_EQ(*json.boolValue, true);
    ASSERT_EQ(*json.charValue, -1);
    ASSERT_EQ(*json.ucharValue, 2);
    ASSERT_EQ(*json.shortValue, -3);
    ASSERT_EQ(*json.ushortValue, 4);
    ASSERT_EQ(*json.intValue, -5);
    ASSERT_EQ(*json.uintValue, 6);
    ASSERT_EQ(*json.longValue, -7);
    ASSERT_EQ(*json.ulongValue, 8);
    ASSERT_EQ(*json.llongValue, -9);
    ASSERT_EQ(*json.ullongValue, 10);
    ASSERT_EQ(*json.floatValue, 1.5);
    ASSERT_EQ(*json.doubleValue, 2.25);
    ASSERT_EQ(*json.stringValue, "s");
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), SILNGLE_VALUES_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(SILNGLE_VALUES_JSON));
  }
}

XL_JSON_BEGIN(ArrayValues)
  XL_JSON_MEMBER(std::vector<int>, intArray)
  XL_JSON_MEMBER(std::vector<std::string>, stringArray)
XL_JSON_END()

const char *ARRAY_VALUES_JSON = R"({
    "intArray": [
        1,
        2,
        3
    ],
    "stringArray": [
        "a",
        "b",
        "c"
    ]
})";

const char *EMPTY_ARRAY_VALUES_JSON = R"({
    "intArray": [],
    "stringArray": []
})";

TEST(json_test, array_values) {
  {
    ArrayValues json;
    ASSERT_EQ(json.json_parse(ARRAY_VALUES_JSON), true);
    ASSERT_EQ(json.intArray, (std::vector<int>{1, 2, 3}));
    ASSERT_EQ(json.stringArray, (std::vector<std::string>{"a", "b", "c"}));
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), ARRAY_VALUES_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(ARRAY_VALUES_JSON));
  }
  {
    ArrayValues json;
    ASSERT_EQ(json.json_parse(EMPTY_ARRAY_VALUES_JSON), true);
    ASSERT_EQ(json.intArray, (std::vector<int>{}));
    ASSERT_EQ(json.stringArray, (std::vector<std::string>{}));
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), EMPTY_ARRAY_VALUES_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(EMPTY_ARRAY_VALUES_JSON));
  }
}

XL_JSON_BEGIN(ArrayNullableValues)
  XL_JSON_MEMBER(std::vector<std::unique_ptr<int>>, intArray)
  XL_JSON_MEMBER(std::vector<std::unique_ptr<std::string>>, stringArray)
XL_JSON_END()

const char *ARRAY_NULL_VALUES_JSON = R"({
    "intArray": [
        null,
        null
    ],
    "stringArray": [
        null,
        null
    ]
})";
const char *ARRAY_HALF_NULL_VALUES_JSON = R"({
    "intArray": [
        1,
        null
    ],
    "stringArray": [
        null,
        "b"
    ]
})";

TEST(json_test, array_nullble_values) {
  {
    ArrayNullableValues json;
    ASSERT_EQ(json.json_parse(ARRAY_VALUES_JSON), true);
    ASSERT_EQ(json.intArray.size(), 3);
    {
      auto it = json.intArray.begin();
      ASSERT_EQ(**it++, 1);
      ASSERT_EQ(**it++, 2);
      ASSERT_EQ(**it++, 3);
    }
    ASSERT_EQ(json.stringArray.size(), 3);
    {
      auto it = json.stringArray.begin();
      ASSERT_EQ(**it++, "a");
      ASSERT_EQ(**it++, "b");
      ASSERT_EQ(**it++, "c");
    }
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), ARRAY_VALUES_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(ARRAY_VALUES_JSON));
  }
  {
    ArrayNullableValues json;
    ASSERT_EQ(json.json_parse(EMPTY_ARRAY_VALUES_JSON), true);
    ASSERT_EQ(json.intArray.size(), 0);
    ASSERT_EQ(json.stringArray.size(), 0);
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), EMPTY_ARRAY_VALUES_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(EMPTY_ARRAY_VALUES_JSON));
  }
  {
    ArrayNullableValues json;
    ASSERT_EQ(json.json_parse(ARRAY_NULL_VALUES_JSON), true);
    ASSERT_EQ(json.intArray.size(), 2);
    {
      auto it = json.intArray.begin();
      ASSERT_EQ(*it++, nullptr);
      ASSERT_EQ(*it++, nullptr);
    }
    ASSERT_EQ(json.stringArray.size(), 2);
    {
      auto it = json.stringArray.begin();
      ASSERT_EQ(*it++, nullptr);
      ASSERT_EQ(*it++, nullptr);
    }
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), ARRAY_NULL_VALUES_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(ARRAY_NULL_VALUES_JSON));
  }
  {
    ArrayNullableValues json;
    ASSERT_EQ(json.json_parse(ARRAY_HALF_NULL_VALUES_JSON), true);
    ASSERT_EQ(json.intArray.size(), 2);
    {
      auto it = json.intArray.begin();
      ASSERT_EQ(**it++, 1);
      ASSERT_EQ(*it++, nullptr);
    }
    ASSERT_EQ(json.stringArray.size(), 2);
    {
      auto it = json.stringArray.begin();
      ASSERT_EQ(*it++, nullptr);
      ASSERT_EQ(**it++, "b");
    }
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), ARRAY_HALF_NULL_VALUES_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(ARRAY_HALF_NULL_VALUES_JSON));
  }
}

XL_JSON_BEGIN(NullableArrayValues)
  XL_JSON_MEMBER(std::unique_ptr<std::vector<int>>, intArray)
  XL_JSON_MEMBER(std::unique_ptr<std::vector<std::string>>, stringArray)
XL_JSON_END()

const char *NULL_ARRAY_VALUES_JSON = R"({
    "intArray": null,
    "stringArray": null
})";

const char *HALF_NULL_ARRAY_VALUES_JSON = R"({
    "intArray": null,
    "stringArray": [
        "a",
        "b"
    ]
})";

TEST(json_test, nullable_array_values) {
  {
    NullableArrayValues json;
    ASSERT_EQ(json.json_parse(ARRAY_VALUES_JSON), true);
    ASSERT_EQ(json.intArray->size(), 3);
    {
      auto it = json.intArray->begin();
      ASSERT_EQ(*it++, 1);
      ASSERT_EQ(*it++, 2);
      ASSERT_EQ(*it++, 3);
    }
    ASSERT_EQ(json.stringArray->size(), 3);
    {
      auto it = json.stringArray->begin();
      ASSERT_EQ(*it++, "a");
      ASSERT_EQ(*it++, "b");
      ASSERT_EQ(*it++, "c");
    }
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), ARRAY_VALUES_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(ARRAY_VALUES_JSON));
  }
  {
    NullableArrayValues json;
    ASSERT_EQ(json.json_parse(EMPTY_ARRAY_VALUES_JSON), true);
    ASSERT_EQ(json.intArray->size(), 0);
    ASSERT_EQ(json.stringArray->size(), 0);
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), EMPTY_ARRAY_VALUES_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(EMPTY_ARRAY_VALUES_JSON));
  }
  {
    NullableArrayValues json;
    ASSERT_EQ(json.json_parse(NULL_ARRAY_VALUES_JSON), true);
    ASSERT_EQ(json.intArray, nullptr);
    ASSERT_EQ(json.stringArray, nullptr);
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), EMPTY_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(EMPTY_JSON));
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY | ::xl::json::WRITE_FLAG_WRITE_NULL_VALUES),
              NULL_ARRAY_VALUES_JSON);
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_WRITE_NULL_VALUES), remove_blanks(NULL_ARRAY_VALUES_JSON));
  }
  {
    NullableArrayValues json;
    ASSERT_EQ(json.json_parse(HALF_NULL_ARRAY_VALUES_JSON), true);
    ASSERT_EQ(json.intArray, nullptr);
    ASSERT_EQ(*json.stringArray, (std::vector<std::string>{"a", "b"}));
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), R"({
    "stringArray": [
        "a",
        "b"
    ]
})");
    ASSERT_EQ(json.json_dump(), remove_blanks(R"({
    "stringArray": [
        "a",
        "b"
    ]
})"));
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY | ::xl::json::WRITE_FLAG_WRITE_NULL_VALUES),
              HALF_NULL_ARRAY_VALUES_JSON);
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_WRITE_NULL_VALUES), remove_blanks(HALF_NULL_ARRAY_VALUES_JSON));
  }
}

XL_JSON_BEGIN(NullbleArrayNullableValues)
  XL_JSON_MEMBER(std::unique_ptr<std::vector<std::unique_ptr<int>>>, intArray)
  XL_JSON_MEMBER(std::unique_ptr<std::vector<std::unique_ptr<std::string>>>, stringArray)
XL_JSON_END()

const char *HALF_NULL_ARRAY_HALF_NULL_VALUES_JSON = R"({
    "intArray": null,
    "stringArray": [
        null,
        "b"
    ]
})";

TEST(json_test, nullable_array_nullble_values) {
  {
    NullbleArrayNullableValues json;
    ASSERT_EQ(json.json_parse(ARRAY_VALUES_JSON), true);
    ASSERT_EQ(json.intArray->size(), 3);
    {
      auto it = json.intArray->begin();
      ASSERT_EQ(**it++, 1);
      ASSERT_EQ(**it++, 2);
      ASSERT_EQ(**it++, 3);
    }
    ASSERT_EQ(json.stringArray->size(), 3);
    {
      auto it = json.stringArray->begin();
      ASSERT_EQ(**it++, "a");
      ASSERT_EQ(**it++, "b");
      ASSERT_EQ(**it++, "c");
    }
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), ARRAY_VALUES_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(ARRAY_VALUES_JSON));
  }
  {
    NullbleArrayNullableValues json;
    ASSERT_EQ(json.json_parse(EMPTY_ARRAY_VALUES_JSON), true);
    ASSERT_EQ(json.intArray->size(), 0);
    ASSERT_EQ(json.stringArray->size(), 0);
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), EMPTY_ARRAY_VALUES_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(EMPTY_ARRAY_VALUES_JSON));
  }
  {
    NullbleArrayNullableValues json;
    ASSERT_EQ(json.json_parse(ARRAY_NULL_VALUES_JSON), true);
    ASSERT_EQ(json.intArray->size(), 2);
    {
      auto it = json.intArray->begin();
      ASSERT_EQ(*it++, nullptr);
      ASSERT_EQ(*it++, nullptr);
    }
    ASSERT_EQ(json.stringArray->size(), 2);
    {
      auto it = json.stringArray->begin();
      ASSERT_EQ(*it++, nullptr);
      ASSERT_EQ(*it++, nullptr);
    }
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), ARRAY_NULL_VALUES_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(ARRAY_NULL_VALUES_JSON));
  }
  {
    NullbleArrayNullableValues json;
    ASSERT_EQ(json.json_parse(ARRAY_HALF_NULL_VALUES_JSON), true);
    ASSERT_EQ(json.intArray->size(), 2);
    {
      auto it = json.intArray->begin();
      ASSERT_EQ(**it++, 1);
      ASSERT_EQ(*it++, nullptr);
    }
    ASSERT_EQ(json.stringArray->size(), 2);
    {
      auto it = json.stringArray->begin();
      ASSERT_EQ(*it++, nullptr);
      ASSERT_EQ(**it++, "b");
    }
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), ARRAY_HALF_NULL_VALUES_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(ARRAY_HALF_NULL_VALUES_JSON));
  }
  {
    NullbleArrayNullableValues json;
    ASSERT_EQ(json.json_parse(NULL_ARRAY_VALUES_JSON), true);
    ASSERT_EQ(json.intArray, nullptr);
    ASSERT_EQ(json.stringArray, nullptr);
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), EMPTY_JSON);
    ASSERT_EQ(json.json_dump(), remove_blanks(EMPTY_JSON));
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY | ::xl::json::WRITE_FLAG_WRITE_NULL_VALUES),
              NULL_ARRAY_VALUES_JSON);
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_WRITE_NULL_VALUES), remove_blanks(NULL_ARRAY_VALUES_JSON));
  }
  {
    NullbleArrayNullableValues json;
    ASSERT_EQ(json.json_parse(HALF_NULL_ARRAY_VALUES_JSON), true);
    ASSERT_EQ(json.intArray, nullptr);
    {
      auto it = json.stringArray->begin();
      ASSERT_EQ(**it++, "a");
      ASSERT_EQ(**it++, "b");
    }
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), R"({
    "stringArray": [
        "a",
        "b"
    ]
})");
    ASSERT_EQ(json.json_dump(), remove_blanks(R"({
    "stringArray": [
        "a",
        "b"
    ]
})"));
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY | ::xl::json::WRITE_FLAG_WRITE_NULL_VALUES),
              HALF_NULL_ARRAY_VALUES_JSON);
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_WRITE_NULL_VALUES), remove_blanks(HALF_NULL_ARRAY_VALUES_JSON));
  }
  {
    NullbleArrayNullableValues json;
    ASSERT_EQ(json.json_parse(HALF_NULL_ARRAY_HALF_NULL_VALUES_JSON), true);
    ASSERT_EQ(json.intArray, nullptr);
    {
      auto it = json.stringArray->begin();
      ASSERT_EQ(*it++, nullptr);
      ASSERT_EQ(**it++, "b");
    }
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), R"({
    "stringArray": [
        null,
        "b"
    ]
})");
    ASSERT_EQ(json.json_dump(), remove_blanks(R"({
    "stringArray": [
        null,
        "b"
    ]
})"));
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY | ::xl::json::WRITE_FLAG_WRITE_NULL_VALUES),
              HALF_NULL_ARRAY_HALF_NULL_VALUES_JSON);
    ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_WRITE_NULL_VALUES),
              remove_blanks(HALF_NULL_ARRAY_HALF_NULL_VALUES_JSON));
  }
}

XL_JSON_BEGIN(SimpleObject)
  XL_JSON_MEMBER(int, intValue)
XL_JSON_END()

XL_JSON_BEGIN(NestObjectValues)
  XL_JSON_MEMBER(SimpleObject, nestObject)
  XL_JSON_MEMBER(std::unique_ptr<SimpleObject>, nestNullableObject)
  XL_JSON_MEMBER(std::vector<SimpleObject>, nestObjectArray)
XL_JSON_END()

const char *NEST_OBJECT_JSON = R"({
    "nestObject": {
        "intValue": 123
    },
    "nestNullableObject": {
        "intValue": 456
    },
    "nestObjectArray": [
        {
            "intValue": 789
        },
        {
            "intValue": 10
        }
    ]
})";

TEST(json_test, nest_object_values) {
  NestObjectValues json;
  ASSERT_EQ(json.json_parse(NEST_OBJECT_JSON), true);
  ASSERT_EQ(json.nestObject.intValue, 123);
  ASSERT_EQ(json.nestNullableObject->intValue, 456);
  ASSERT_EQ(json.nestObjectArray.front().intValue, 789);
  ASSERT_EQ(json.nestObjectArray.back().intValue, 10);
  ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), NEST_OBJECT_JSON);
  ASSERT_EQ(json.json_dump(), remove_blanks(NEST_OBJECT_JSON));
}

TEST(json_test, copy_and_move) {
  SingleValues json;
  ASSERT_EQ(json.json_parse(SILNGLE_VALUES_JSON), true);
  ASSERT_EQ(json.json_dump(::xl::json::WRITE_FLAG_PRETTY), SILNGLE_VALUES_JSON);
  SingleValues json2 = json;
  ASSERT_EQ(json2.json_dump(::xl::json::WRITE_FLAG_PRETTY), SILNGLE_VALUES_JSON);
  SingleValues json3 = std::move(json2);
  ASSERT_EQ(json3.json_dump(::xl::json::WRITE_FLAG_PRETTY), SILNGLE_VALUES_JSON);
}
