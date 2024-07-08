#include <gtest/gtest.h>
#include <xl/ini>

namespace {

const char *CONTENT = "[section1]    ;section1 comment\r\n"
                      "key1 = value1 ;comment1\r\n"
                      "key2 = value2 ;comment2\r\n"
                      "\r\n"
                      "[section2]    ;section2 comment\r\n"
                      "key3 = value3 ;comment3\r\n"
                      "\r\n";

} // namespace

TEST(ini_test, normal) {
  xl::ini ini;
  ASSERT_EQ(ini.parse(CONTENT), true);
  xl::ini::ini_data data = ini.data();
  ASSERT_EQ(data.sections.size(), 2);
  {
    const auto &section = data.sections.front();
    ASSERT_EQ(section.name, "section1");
    ASSERT_EQ(section.comment, "section1 comment");
    ASSERT_EQ(section.lines.size(), 2);
    {
      const auto &line = section.lines.front();
      ASSERT_EQ(line.key, "key1");
      ASSERT_EQ(line.value, "value1");
      ASSERT_EQ(line.comment, "comment1");
    }
    {
      const auto &line = section.lines.back();
      ASSERT_EQ(line.key, "key2");
      ASSERT_EQ(line.value, "value2");
      ASSERT_EQ(line.comment, "comment2");
    }
  }
  {
    const auto &section = data.sections.back();
    ASSERT_EQ(section.name, "section2");
    ASSERT_EQ(section.comment, "section2 comment");
    ASSERT_EQ(section.lines.size(), 1);
    {
      const auto &line = section.lines.front();
      ASSERT_EQ(line.key, "key3");
      ASSERT_EQ(line.value, "value3");
      ASSERT_EQ(line.comment, "comment3");
    }
  }
  ASSERT_EQ(ini.dump(), CONTENT);
}

TEST(ini_test, global_section) {
  const char *content = "; comment line\r\n"
                        "key1 = value1 ; comment1\r\n"
                        "\r\n"
                        "[section2]    ; section2 comment\r\n"
                        "key2 = value2 ; comment2\r\n"
                        "\r\n";

  xl::ini ini;
  ASSERT_EQ(ini.parse(content), true);
  xl::ini::ini_data data = ini.data();
  ASSERT_EQ(data.sections.size(), 2);
  {
    const auto &section = data.sections.front();
    ASSERT_EQ(section.name, "");
    ASSERT_EQ(section.comment, "");
    ASSERT_EQ(section.lines.size(), 2);
    {
      const auto &line = section.lines.front();
      ASSERT_EQ(line.key, "");
      ASSERT_EQ(line.value, "");
      ASSERT_EQ(line.comment, " comment line");
    }
    {
      const auto &line = section.lines.back();
      ASSERT_EQ(line.key, "key1");
      ASSERT_EQ(line.value, "value1");
      ASSERT_EQ(line.comment, " comment1");
    }
  }
  {
    const auto &section = data.sections.back();
    ASSERT_EQ(section.name, "section2");
    ASSERT_EQ(section.comment, " section2 comment");
    ASSERT_EQ(section.lines.size(), 1);
    {
      const auto &line = section.lines.front();
      ASSERT_EQ(line.key, "key2");
      ASSERT_EQ(line.value, "value2");
      ASSERT_EQ(line.comment, " comment2");
    }
  }
  ASSERT_EQ(ini.dump(), content);
}

TEST(ini_test, duplicated_sections_and_keys) {
  const char *content = "[section1]     ;section1 comment\r\n"
                        "key1 = value11 ;comment11\r\n"
                        "key1 = value12 ;comment12\r\n"
                        "\r\n"
                        "[section2]     ;section2 comment1\r\n"
                        "key2 = value21 ;comment21\r\n"
                        "\r\n"
                        "[section2]     ;section2 comment2\r\n"
                        "key2 = value22 ;comment22\r\n"
                        "\r\n";

  xl::ini ini;
  ASSERT_EQ(ini.parse(content), true);
  xl::ini::ini_data data = ini.data();
  ASSERT_EQ(data.sections.size(), 3);
  {
    const auto &section = data.sections.front();
    ASSERT_EQ(section.name, "section1");
    ASSERT_EQ(section.comment, "section1 comment");
    ASSERT_EQ(section.lines.size(), 2);
    {
      const auto &line = section.lines.front();
      ASSERT_EQ(line.key, "key1");
      ASSERT_EQ(line.value, "value11");
      ASSERT_EQ(line.comment, "comment11");
    }
    {
      const auto &line = section.lines.back();
      ASSERT_EQ(line.key, "key1");
      ASSERT_EQ(line.value, "value12");
      ASSERT_EQ(line.comment, "comment12");
    }
  }
  {
    const auto &section = *(++data.sections.begin());
    ASSERT_EQ(section.name, "section2");
    ASSERT_EQ(section.comment, "section2 comment1");
    ASSERT_EQ(section.lines.size(), 1);
    {
      const auto &line = section.lines.front();
      ASSERT_EQ(line.key, "key2");
      ASSERT_EQ(line.value, "value21");
      ASSERT_EQ(line.comment, "comment21");
    }
  }
  {
    const auto &section = data.sections.back();
    ASSERT_EQ(section.name, "section2");
    ASSERT_EQ(section.comment, "section2 comment2");
    ASSERT_EQ(section.lines.size(), 1);
    {
      const auto &line = section.lines.back();
      ASSERT_EQ(line.key, "key2");
      ASSERT_EQ(line.value, "value22");
      ASSERT_EQ(line.comment, "comment22");
    }
  }
  ASSERT_EQ(ini.dump(), content);
}

TEST(ini_test, dump_pretty) {
  const char *content = "[section1];section1 comment\r\n"
                        "key1=value1;comment1\r\n"
                        "key2=value2;comment2\r\n"
                        "\r\n"
                        "   [   section2   ]       ;section2 comment\r\n"
                        "   key3    =    value3    ;comment3\r\n"
                        "   \r\n   ";

  xl::ini ini;
  ASSERT_EQ(ini.parse(content), true);
  xl::ini::ini_data data = ini.data();
  ASSERT_EQ(data.sections.size(), 2);
  {
    const auto &section = data.sections.front();
    ASSERT_EQ(section.name, "section1");
    ASSERT_EQ(section.comment, "section1 comment");
    ASSERT_EQ(section.lines.size(), 2);
    {
      const auto &line = section.lines.front();
      ASSERT_EQ(line.key, "key1");
      ASSERT_EQ(line.value, "value1");
      ASSERT_EQ(line.comment, "comment1");
    }
    {
      const auto &line = section.lines.back();
      ASSERT_EQ(line.key, "key2");
      ASSERT_EQ(line.value, "value2");
      ASSERT_EQ(line.comment, "comment2");
    }
  }
  {
    const auto &section = data.sections.back();
    ASSERT_EQ(section.name, "section2");
    ASSERT_EQ(section.comment, "section2 comment");
    ASSERT_EQ(section.lines.size(), 1);
    {
      const auto &line = section.lines.front();
      ASSERT_EQ(line.key, "key3");
      ASSERT_EQ(line.value, "value3");
      ASSERT_EQ(line.comment, "comment3");
    }
  }
  ASSERT_EQ(ini.dump(), CONTENT);
}
