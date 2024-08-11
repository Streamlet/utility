#include <gtest/gtest.h>
#include <xl/file>
#include <xl/zip>

TEST(zip_test, single_file) {
  xl::fs::remove_all(_T("test_root"));

  ASSERT_EQ(xl::fs::mkdirs(_T("test_root")), true);
  ASSERT_EQ(xl::file::write(xl::path::join(_T("test_root"), _T("f1")).c_str(), "content"), true);

  ASSERT_EQ(xl::zip::compress(xl::path::join(_T("test_root"), _T("test.zip")).c_str(),
                              xl::path::join(_T("test_root"), _T("f1")).c_str()),
            true);
  ASSERT_EQ(xl::zip::read_file(xl::path::join(_T("test_root"), _T("test.zip")).c_str(), "f1"), "content");
  ASSERT_EQ(xl::zip::extract(xl::path::join(_T("test_root"), _T("test.zip")).c_str(),
                             xl::path::join(_T("test_root"), _T("unzip")).c_str()),

            true);
  ASSERT_EQ(xl::file::read(xl::path::join(_T("test_root"), _T("unzip"), _T("f1")).c_str()), "content");

  ASSERT_EQ(xl::fs::remove_all(_T("test_root")), true);
}

TEST(zip_test, single_directory) {
  xl::fs::remove_all(_T("test_root"));

  ASSERT_EQ(xl::fs::mkdirs(xl::path::join(_T("test_root"), _T("d1")).c_str()), true);
  ASSERT_EQ(xl::file::write(xl::path::join(_T("test_root"), _T("d1"), _T("f1")).c_str(), "content"), true);
  ASSERT_EQ(xl::zip::compress(xl::path::join(_T("test_root"), _T("test.zip")).c_str(),
                              xl::path::join(_T("test_root"), _T("d1")).c_str()),
            true);
  ASSERT_EQ(xl::zip::read_file(xl::path::join(_T("test_root"), _T("test.zip")).c_str(), "d1/f1"), "content");
  ASSERT_EQ(xl::zip::extract(xl::path::join(_T("test_root"), _T("test.zip")).c_str(),
                             xl::path::join(_T("test_root"), _T("unzip")).c_str()),
            true);
  ASSERT_EQ(xl::file::read(xl::path::join(_T("test_root"), _T("unzip"), _T("d1"), _T("f1")).c_str()), "content");

  ASSERT_EQ(xl::fs::remove_all(_T("test_root")), true);
}

TEST(zip_test, single_directory_content) {
  xl::fs::remove_all(_T("test_root"));

  ASSERT_EQ(xl::fs::mkdirs(xl::path::join(_T("test_root"), _T("d1")).c_str()), true);
  ASSERT_EQ(xl::file::write(xl::path::join(_T("test_root"), _T("d1"), _T("f1")).c_str(), "content"), true);
  ASSERT_EQ(xl::zip::compress_folder_content(xl::path::join(_T("test_root"), _T("test.zip")).c_str(),
                                             xl::path::join(_T("test_root"), _T("d1")).c_str()),
            true);
  ASSERT_EQ(xl::zip::read_file(xl::path::join(_T("test_root"), _T("test.zip")).c_str(), "f1"), "content");
  ASSERT_EQ(xl::zip::extract(xl::path::join(_T("test_root"), _T("test.zip")).c_str(),
                             xl::path::join(_T("test_root"), _T("unzip")).c_str()),
            true);
  ASSERT_EQ(xl::file::read(xl::path::join(_T("test_root"), _T("unzip"), _T("f1")).c_str()), "content");

  ASSERT_EQ(xl::fs::remove_all(_T("test_root")), true);
}

TEST(zip_test, single_directory_contruct) {
  xl::fs::remove_all(_T("test_root"));

  ASSERT_EQ(xl::fs::mkdirs(xl::path::join(_T("test_root"), _T("d1")).c_str()), true);
  ASSERT_EQ(xl::file::write(xl::path::join(_T("test_root"), _T("d1"), _T("f1")).c_str(), "content"), true);

  ASSERT_EQ(xl::zip::touch(xl::path::join(_T("test_root"), _T("test.zip")).c_str()), true);
  ASSERT_EQ(xl::zip::add_file(xl::path::join(_T("test_root"), _T("test.zip")).c_str(), "d1/f1",
                              xl::path::join(_T("test_root"), _T("d1"), _T("f1")).c_str()),
            true);
  ASSERT_EQ(xl::zip::add_file(xl::path::join(_T("test_root"), _T("test.zip")).c_str(), "d1/f2", "data", 4), true);
  ASSERT_EQ(xl::zip::add_file(xl::path::join(_T("test_root"), _T("test.zip")).c_str(), "d2",
                              xl::path::join(_T("test_root"), _T("d1")).c_str()),
            true);
  ASSERT_EQ(xl::zip::read_file(xl::path::join(_T("test_root"), _T("test.zip")).c_str(), "d1/f1"), "content");
  ASSERT_EQ(xl::zip::read_file(xl::path::join(_T("test_root"), _T("test.zip")).c_str(), "d1/f2"), "data");
  ASSERT_EQ(xl::zip::read_file(xl::path::join(_T("test_root"), _T("test.zip")).c_str(), "d2/f1"), "content");
  ASSERT_EQ(xl::zip::extract(xl::path::join(_T("test_root"), _T("test.zip")).c_str(),
                             xl::path::join(_T("test_root"), _T("unzip")).c_str()),
            true);
  ASSERT_EQ(xl::file::read(xl::path::join(_T("test_root"), _T("unzip"), _T("d1"), _T("f1")).c_str()), "content");
  ASSERT_EQ(xl::file::read(xl::path::join(_T("test_root"), _T("unzip"), _T("d1"), _T("f2")).c_str()), "data");
  ASSERT_EQ(xl::file::read(xl::path::join(_T("test_root"), _T("unzip"), _T("d2"), _T("f1")).c_str()), "content");

  ASSERT_EQ(xl::fs::remove_all(_T("test_root")), true);
}

TEST(zip_test, directory_tree) {
  xl::fs::remove_all(_T("test_root"));

  ASSERT_EQ(xl::fs::mkdirs(xl::path::join(_T("test_root"), _T("d1"), _T("d2"), _T("d3")).c_str()), true);
  ASSERT_EQ(xl::file::write(xl::path::join(_T("test_root"), _T("d1"), _T("f1")).c_str(), "content1"), true);
  ASSERT_EQ(xl::file::write(xl::path::join(_T("test_root"), _T("d1"), _T("d2"), _T("f2")).c_str(), "content2"), true);
  ASSERT_EQ(
      xl::file::write(xl::path::join(_T("test_root"), _T("d1"), _T("d2"), _T("d3"), _T("f3")).c_str(), "content3"),
      true);
  ASSERT_EQ(xl::zip::compress(xl::path::join(_T("test_root"), _T("test.zip")).c_str(),
                              xl::path::join(_T("test_root"), _T("d1")).c_str()),
            true);
  ASSERT_EQ(xl::zip::read_file(xl::path::join(_T("test_root"), _T("test.zip")).c_str(), "d1/f1"), "content1");
  ASSERT_EQ(xl::zip::read_file(xl::path::join(_T("test_root"), _T("test.zip")).c_str(), "d1/d2/f2"), "content2");
  ASSERT_EQ(xl::zip::read_file(xl::path::join(_T("test_root"), _T("test.zip")).c_str(), "d1/d2/d3/f3"), "content3");

  ASSERT_EQ(xl::zip::extract(xl::path::join(_T("test_root"), _T("test.zip")).c_str(),
                             xl::path::join(_T("test_root"), _T("unzip")).c_str()),
            true);
  ASSERT_EQ(xl::file::read(xl::path::join(_T("test_root"), _T("unzip"), _T("d1"), _T("f1")).c_str()), "content1");
  ASSERT_EQ(xl::file::read(xl::path::join(_T("test_root"), _T("unzip"), _T("d1"), _T("d2"), _T("f2")).c_str()),
            "content2");
  ASSERT_EQ(
      xl::file::read(xl::path::join(_T("test_root"), _T("unzip"), _T("d1"), _T("d2"), _T("d3"), _T("f3")).c_str()),
      "content3");

  ASSERT_EQ(xl::fs::remove_all(_T("test_root")), true);
}

TEST(zip_test, non_ascii_file_name) {
  xl::fs::remove_all(_T("test_root"));

  ASSERT_EQ(xl::fs::mkdirs(xl::path::join(_T("test_root"), _T("目录1"), _T("目录2"), _T("目录3")).c_str()), true);
  ASSERT_EQ(xl::file::write(xl::path::join(_T("test_root"), _T("目录1"), _T("文件1")).c_str(), "内容1"), true);
  ASSERT_EQ(xl::file::write(xl::path::join(_T("test_root"), _T("目录1"), _T("目录2"), _T("文件2")).c_str(), "内容2"),
            true);
  ASSERT_EQ(xl::file::write(xl::path::join(_T("test_root"), _T("目录1"), _T("目录2"), _T("目录3"), _T("文件3")).c_str(),
                            "内容3"),
            true);
  ASSERT_EQ(xl::zip::compress(xl::path::join(_T("test_root"), _T("测试.zip")).c_str(),
                              xl::path::join(_T("test_root"), _T("目录1")).c_str()),
            true);
  ASSERT_EQ(xl::zip::read_file(xl::path::join(_T("test_root"), _T("测试.zip")).c_str(), "目录1/文件1"), "内容1");
  ASSERT_EQ(xl::zip::read_file(xl::path::join(_T("test_root"), _T("测试.zip")).c_str(), "目录1/目录2/文件2"), "内容2");
  ASSERT_EQ(xl::zip::read_file(xl::path::join(_T("test_root"), _T("测试.zip")).c_str(), "目录1/目录2/目录3/文件3"),
            "内容3");

  ASSERT_EQ(xl::zip::extract(xl::path::join(_T("test_root"), _T("测试.zip")).c_str(),
                             xl::path::join(_T("test_root"), _T("解压")).c_str()),
            true);
  ASSERT_EQ(xl::file::read(xl::path::join(_T("test_root"), _T("解压"), _T("目录1"), _T("文件1")).c_str()), "内容1");
  ASSERT_EQ(xl::file::read(xl::path::join(_T("test_root"), _T("解压"), _T("目录1"), _T("目录2"), _T("文件2")).c_str()),
            "内容2");
  ASSERT_EQ(
      xl::file::read(
          xl::path::join(_T("test_root"), _T("解压"), _T("目录1"), _T("目录2"), _T("目录3"), _T("文件3")).c_str()),
      "内容3");
  ASSERT_EQ(xl::fs::remove_all(_T("test_root")), true);
}
