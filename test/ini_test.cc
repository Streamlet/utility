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
                        "key2 = value2 ;comment2\r\n"
                        "\r\n"
                        "[section2]     ;section2 comment2\r\n"
                        "key3 = value3 ;comment3\r\n"
                        "\r\n";

  xl::ini ini;
  ASSERT_EQ(ini.parse(content), true);
  xl::ini::ini_data data = ini.data();
  ASSERT_EQ(data.sections.size(), 2);
  {
    const auto &section = data.sections.front();
    ASSERT_EQ(section.name, "section1");
    ASSERT_EQ(section.comment, "section1 comment");
    ASSERT_EQ(section.lines.size(), 1);
    {
      const auto &line = section.lines.front();
      ASSERT_EQ(line.key, "key1");
      ASSERT_EQ(line.value, "value12");
      ASSERT_EQ(line.comment, "comment12");
    }
  }
  {
    const auto &section = *(++data.sections.begin());
    ASSERT_EQ(section.name, "section2");
    ASSERT_EQ(section.comment, "section2 comment2");
    ASSERT_EQ(section.lines.size(), 2);
    {
      const auto &line = section.lines.front();
      ASSERT_EQ(line.key, "key2");
      ASSERT_EQ(line.value, "value2");
      ASSERT_EQ(line.comment, "comment2");
    }
    {
      const auto &line = section.lines.back();
      ASSERT_EQ(line.key, "key3");
      ASSERT_EQ(line.value, "value3");
      ASSERT_EQ(line.comment, "comment3");
    }
  }
  ASSERT_EQ(ini.dump(), "[section1]     ;section1 comment\r\n"
                        "key1 = value12 ;comment12\r\n"
                        "\r\n"
                        "[section2]    ;section2 comment2\r\n"
                        "key2 = value2 ;comment2\r\n"
                        "key3 = value3 ;comment3\r\n"
                        "\r\n");
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

TEST(ini_test, read_api) {
  xl::ini ini;
  ASSERT_EQ(ini.parse(CONTENT), true);
  ASSERT_EQ(ini.has_section("section0"), false);
  ASSERT_EQ(ini.has_section("section1"), true);
  ASSERT_EQ(ini.has_section("section2"), true);
  ASSERT_EQ(ini.has_section("section3"), false);
  ASSERT_EQ(ini.enum_sections(), (std::vector<std::string>{"section1", "section2"}));

  ASSERT_EQ(ini.has_key("section0", "key1"), false);
  ASSERT_EQ(ini.has_key("section1", "key1"), true);
  ASSERT_EQ(ini.has_key("section2", "key1"), false);
  ASSERT_EQ(ini.has_key("section0", "key2"), false);
  ASSERT_EQ(ini.has_key("section1", "key2"), true);
  ASSERT_EQ(ini.has_key("section2", "key2"), false);
  ASSERT_EQ(ini.has_key("section0", "key3"), false);
  ASSERT_EQ(ini.has_key("section1", "key3"), false);
  ASSERT_EQ(ini.has_key("section2", "key3"), true);
  ASSERT_EQ(ini.has_key("section0", "key4"), false);
  ASSERT_EQ(ini.has_key("section1", "key4"), false);
  ASSERT_EQ(ini.has_key("section2", "key4"), false);
  ASSERT_EQ(ini.enum_keys("section0"), (std::vector<std::string>{}));
  ASSERT_EQ(ini.enum_keys("section1"), (std::vector<std::string>{"key1", "key2"}));
  ASSERT_EQ(ini.enum_keys("section2"), (std::vector<std::string>{"key3"}));
  ASSERT_EQ(ini.enum_keys("section3"), (std::vector<std::string>{}));

  ASSERT_EQ(ini.get_value("section0", "key1"), "");
  ASSERT_EQ(ini.get_value("section1", "key1"), "value1");
  ASSERT_EQ(ini.get_value("section2", "key1"), "");
  ASSERT_EQ(ini.get_value("section0", "key2"), "");
  ASSERT_EQ(ini.get_value("section1", "key2"), "value2");
  ASSERT_EQ(ini.get_value("section2", "key2"), "");
  ASSERT_EQ(ini.get_value("section0", "key3"), "");
  ASSERT_EQ(ini.get_value("section1", "key3"), "");
  ASSERT_EQ(ini.get_value("section2", "key3"), "value3");
  ASSERT_EQ(ini.get_value("section0", "key4"), "");
  ASSERT_EQ(ini.get_value("section1", "key4"), "");
  ASSERT_EQ(ini.get_value("section2", "key4"), "");

  ASSERT_EQ(ini.dump(), CONTENT);
}

TEST(ini_test, write_api) {
  xl::ini ini;
  ini.add_section("section1", "section1 comment");
  ini.set_value("section1", "key1", "value1", "comment1");
  ini.set_value("section1", "key2", "value2", "comment2");
  ini.add_section("section2", "section2 comment");
  ini.set_value("section2", "key3", "value3", "comment3");
  ASSERT_EQ(ini.dump(), CONTENT);
}