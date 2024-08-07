#include <gtest/gtest.h>
#include <xl/file>
#include <xl/zip>

TEST(zip_test, single_file) {
  // ASSERT_EQ(xl::fs::mkdirs(_T("test_root")), true);
  // ASSERT_EQ(xl::file::write(_T("test_root") XL_FS_SEP _T("f1"), "content"), true);
  // ASSERT_EQ(xl::zip::zip_compress(_T("test_root") XL_FS_SEP _T("test.zip"), _T("test_root") XL_FS_SEP _T("f1")),
  // true); ASSERT_EQ(xl::zip::zip_read_file(_T("test_root") XL_FS_SEP _T("test.zip"), "f1"), "content");
  // ASSERT_EQ(xl::zip::zip_extract(_T("test_root") XL_FS_SEP _T("test.zip"), _T("test_root") XL_FS_SEP _T("unzip")),
  //           true);
  // ASSERT_EQ(xl::file::read(_T("test_root") XL_FS_SEP _T("unzip") XL_FS_SEP _T("f1")), "content");
}
