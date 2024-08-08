#include <gtest/gtest.h>
#include <xl/file>
#include <xl/zip>

TEST(zip_test, single_file) {
  xl::fs::remove_all(_T("test_root"));

  ASSERT_EQ(xl::fs::mkdirs(_T("test_root")), true);
  ASSERT_EQ(xl::file::write(_T("test_root") XL_FS_SEP _T("f1"), "content"), true);

  ASSERT_EQ(xl::zip::zip_compress(_T("test_root") XL_FS_SEP _T("test.zip"), _T("test_root") XL_FS_SEP _T("f1")), true);
  ASSERT_EQ(xl::zip::zip_read_file(_T("test_root") XL_FS_SEP _T("test.zip"), "f1"), "content");
  ASSERT_EQ(xl::zip::zip_extract(_T("test_root") XL_FS_SEP _T("test.zip"), _T("test_root") XL_FS_SEP _T("unzip")),
            true);
  ASSERT_EQ(xl::file::read(_T("test_root") XL_FS_SEP _T("unzip") XL_FS_SEP _T("f1")), "content");

  ASSERT_EQ(xl::fs::remove_all(_T("test_root")), true);
}

TEST(zip_test, single_directory) {
  xl::fs::remove_all(_T("test_root"));

  ASSERT_EQ(xl::fs::mkdirs(_T("test_root") XL_FS_SEP _T("d1")), true);
  ASSERT_EQ(xl::file::write(_T("test_root") XL_FS_SEP _T("d1") XL_FS_SEP _T("f1"), "content"), true);
  ASSERT_EQ(xl::zip::zip_compress(_T("test_root") XL_FS_SEP _T("test.zip"), _T("test_root") XL_FS_SEP _T("d1")), true);
  ASSERT_EQ(xl::zip::zip_read_file(_T("test_root") XL_FS_SEP _T("test.zip"), "d1/f1"), "content");
  ASSERT_EQ(xl::zip::zip_extract(_T("test_root") XL_FS_SEP _T("test.zip"), _T("test_root") XL_FS_SEP _T("unzip")),
            true);
  ASSERT_EQ(xl::file::read(_T("test_root") XL_FS_SEP _T("unzip") XL_FS_SEP _T("d1") XL_FS_SEP _T("f1")), "content");

  ASSERT_EQ(xl::fs::remove_all(_T("test_root")), true);
}

TEST(zip_test, directory_tree) {
  xl::fs::remove_all(_T("test_root"));

  ASSERT_EQ(xl::fs::mkdirs(_T("test_root") XL_FS_SEP _T("d1") XL_FS_SEP _T("d2") XL_FS_SEP _T("d3")), true);
  ASSERT_EQ(xl::file::write(_T("test_root") XL_FS_SEP _T("d1") XL_FS_SEP _T("f1"), "content1"), true);
  ASSERT_EQ(xl::file::write(_T("test_root") XL_FS_SEP _T("d1") XL_FS_SEP _T("d2") XL_FS_SEP _T("f2"), "content2"),
            true);
  ASSERT_EQ(xl::file::write(_T("test_root") XL_FS_SEP _T("d1") XL_FS_SEP _T("d2") XL_FS_SEP _T("d3") XL_FS_SEP _T("f3"),
                            "content3"),
            true);
  ASSERT_EQ(xl::zip::zip_compress(_T("test_root") XL_FS_SEP _T("test.zip"), _T("test_root") XL_FS_SEP _T("d1")), true);
  ASSERT_EQ(xl::zip::zip_read_file(_T("test_root") XL_FS_SEP _T("test.zip"), "d1/f1"), "content1");
  ASSERT_EQ(xl::zip::zip_read_file(_T("test_root") XL_FS_SEP _T("test.zip"), "d1/d2/f2"), "content2");
  ASSERT_EQ(xl::zip::zip_read_file(_T("test_root") XL_FS_SEP _T("test.zip"), "d1/d2/d3/f3"), "content3");

  ASSERT_EQ(xl::zip::zip_extract(_T("test_root") XL_FS_SEP _T("test.zip"), _T("test_root") XL_FS_SEP _T("unzip")),
            true);
  ASSERT_EQ(xl::file::read(_T("test_root") XL_FS_SEP _T("unzip") XL_FS_SEP _T("d1") XL_FS_SEP _T("f1")), "content1");
  ASSERT_EQ(
      xl::file::read(_T("test_root") XL_FS_SEP _T("unzip") XL_FS_SEP _T("d1") XL_FS_SEP _T("d2") XL_FS_SEP _T("f2")),
      "content2");
  ASSERT_EQ(xl::file::read(_T("test_root") XL_FS_SEP _T("unzip") XL_FS_SEP _T("d1") XL_FS_SEP _T("d2") XL_FS_SEP
                           _T("d3") XL_FS_SEP _T("f3")),
            "content3");

  ASSERT_EQ(xl::fs::remove_all(_T("test_root")), true);
}

TEST(zip_test, wildcard1) {
  xl::fs::remove_all(_T("test_root"));

  ASSERT_EQ(xl::fs::mkdirs(_T("test_root") XL_FS_SEP _T("d1")), true);
  ASSERT_EQ(xl::file::write(_T("test_root") XL_FS_SEP _T("d1") XL_FS_SEP _T("f1"), "content"), true);

  ASSERT_EQ(xl::zip::zip_compress(_T("test_root") XL_FS_SEP _T("test.zip"),
                                  _T("test_root") XL_FS_SEP _T("d1") XL_FS_SEP _T("*")),
            true);
  ASSERT_EQ(xl::zip::zip_read_file(_T("test_root") XL_FS_SEP _T("test.zip"), "f1"), "content");
  ASSERT_EQ(xl::zip::zip_extract(_T("test_root") XL_FS_SEP _T("test.zip"), _T("test_root") XL_FS_SEP _T("unzip")),
            true);
  ASSERT_EQ(xl::file::read(_T("test_root") XL_FS_SEP _T("unzip") XL_FS_SEP _T("f1")), "content");

  ASSERT_EQ(xl::fs::remove_all(_T("test_root")), true);
}

TEST(zip_test, wildcard2) {
  xl::fs::remove_all(_T("test_root"));

  ASSERT_EQ(xl::fs::mkdirs(_T("test_root") XL_FS_SEP _T("d1") XL_FS_SEP _T("d2") XL_FS_SEP _T("d22")), true);
  ASSERT_EQ(xl::fs::mkdirs(_T("test_root") XL_FS_SEP _T("d1") XL_FS_SEP _T("d2") XL_FS_SEP _T("d33")), true);
  ASSERT_EQ(xl::file::write(_T("test_root") XL_FS_SEP _T("d1") XL_FS_SEP _T("f1"), "content1"), true);
  ASSERT_EQ(xl::file::write(_T("test_root") XL_FS_SEP _T("d1") XL_FS_SEP _T("d2") XL_FS_SEP _T("f2"), "content2"),
            true);
  ASSERT_EQ(xl::file::write(_T("test_root") XL_FS_SEP _T("d1") XL_FS_SEP _T("d2") XL_FS_SEP _T("d22") XL_FS_SEP
                            _T("f22"),
                            "content22"),
            true);
  ASSERT_EQ(xl::file::write(_T("test_root") XL_FS_SEP _T("d1") XL_FS_SEP _T("d2") XL_FS_SEP _T("d33") XL_FS_SEP
                            _T("f33"),
                            "content33"),
            true);
  ASSERT_EQ(xl::zip::zip_compress(_T("test_root") XL_FS_SEP _T("test.zip"),
                                  _T("test_root") XL_FS_SEP _T("d1") XL_FS_SEP _T("*2")),
            true);
  ASSERT_EQ(xl::zip::zip_read_file(_T("test_root") XL_FS_SEP _T("test.zip"), "f1"), "");
  ASSERT_EQ(xl::zip::zip_read_file(_T("test_root") XL_FS_SEP _T("test.zip"), "d2/f2"), "content2");
  ASSERT_EQ(xl::zip::zip_read_file(_T("test_root") XL_FS_SEP _T("test.zip"), "d2/d22/f22"), "content22");
  ASSERT_EQ(xl::zip::zip_read_file(_T("test_root") XL_FS_SEP _T("test.zip"), "d2/d33/f33"), "content33");

  ASSERT_EQ(xl::zip::zip_extract(_T("test_root") XL_FS_SEP _T("test.zip"), _T("test_root") XL_FS_SEP _T("unzip")),
            true);
  ASSERT_EQ(xl::file::read(_T("test_root") XL_FS_SEP _T("unzip") XL_FS_SEP _T("f1")), "");
  ASSERT_EQ(
      xl::file::read(_T("test_root") XL_FS_SEP _T("unzip") XL_FS_SEP _T("d2") XL_FS_SEP _T("d22") XL_FS_SEP _T("f22")),
      "content22");
  ASSERT_EQ(
      xl::file::read(_T("test_root") XL_FS_SEP _T("unzip") XL_FS_SEP _T("d2") XL_FS_SEP _T("d33") XL_FS_SEP _T("f33")),
      "content33");

  ASSERT_EQ(xl::fs::remove_all(_T("test_root")), true);
}

TEST(zip_test, non_ascii_file_name) {
  xl::fs::remove_all(_T("test_root"));

  ASSERT_EQ(xl::fs::mkdirs(_T("test_root") XL_FS_SEP _T("目录1") XL_FS_SEP _T("目录2") XL_FS_SEP _T("目录3")), true);
  ASSERT_EQ(xl::file::write(_T("test_root") XL_FS_SEP _T("目录1") XL_FS_SEP _T("文件1"), "内容1"), true);
  ASSERT_EQ(xl::file::write(_T("test_root") XL_FS_SEP _T("目录1") XL_FS_SEP _T("目录2") XL_FS_SEP _T("文件2"), "内容2"),
            true);
  ASSERT_EQ(xl::file::write(_T("test_root") XL_FS_SEP _T("目录1") XL_FS_SEP _T("目录2") XL_FS_SEP _T("目录3") XL_FS_SEP
                            _T("文件3"),
                            "内容3"),
            true);
  ASSERT_EQ(xl::zip::zip_compress(_T("test_root") XL_FS_SEP _T("测试.zip"), _T("test_root") XL_FS_SEP _T("目录1")),
            true);
  ASSERT_EQ(xl::zip::zip_read_file(_T("test_root") XL_FS_SEP _T("测试.zip"), "目录1/文件1"), "内容1");
  ASSERT_EQ(xl::zip::zip_read_file(_T("test_root") XL_FS_SEP _T("测试.zip"), "目录1/目录2/文件2"), "内容2");
  ASSERT_EQ(xl::zip::zip_read_file(_T("test_root") XL_FS_SEP _T("测试.zip"), "目录1/目录2/目录3/文件3"), "内容3");

  ASSERT_EQ(xl::zip::zip_extract(_T("test_root") XL_FS_SEP _T("测试.zip"), _T("test_root") XL_FS_SEP _T("解压")), true);
  ASSERT_EQ(xl::file::read(_T("test_root") XL_FS_SEP _T("解压") XL_FS_SEP _T("目录1") XL_FS_SEP _T("文件1")), "内容1");
  ASSERT_EQ(xl::file::read(_T("test_root") XL_FS_SEP _T("解压") XL_FS_SEP _T("目录1") XL_FS_SEP _T("目录2") XL_FS_SEP
                           _T("文件2")),
            "内容2");
  ASSERT_EQ(xl::file::read(_T("test_root") XL_FS_SEP _T("解压") XL_FS_SEP _T("目录1") XL_FS_SEP _T("目录2") XL_FS_SEP
                           _T("目录3") XL_FS_SEP _T("文件3")),
            "内容3");

  ASSERT_EQ(xl::fs::remove_all(_T("test_root")), true);
}
