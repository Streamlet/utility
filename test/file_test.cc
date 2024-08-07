#include <gtest/gtest.h>
#include <xl/file>

TEST(file_test, fs_operation) {
  xl::fs::remove(_T("f"));
  xl::fs::remove(_T("f1"));
  xl::fs::remove_all(_T("d"));
  xl::fs::remove_all(_T("d1"));

  ASSERT_EQ(xl::fs::exists(_T("f")), false);
  ASSERT_EQ(xl::fs::size(_T("f")), -1);
  ASSERT_EQ(xl::fs::touch(_T("f")), true);
  ASSERT_EQ(xl::fs::exists(_T("f")), true);
  ASSERT_EQ(xl::fs::size(_T("f")), 0);
  ASSERT_EQ(xl::fs::rename(_T("f"), _T("f1")), true);
  ASSERT_EQ(xl::fs::exists(_T("f")), false);
  ASSERT_EQ(xl::fs::exists(_T("f1")), true);
  ASSERT_EQ(xl::fs::size(_T("f1")), 0);
  ASSERT_EQ(xl::fs::unlink(_T("f1")), true);
  ASSERT_EQ(xl::fs::exists(_T("f")), false);
  ASSERT_EQ(xl::fs::exists(_T("f1")), false);

  ASSERT_EQ(xl::fs::exists(_T("d")), false);
  ASSERT_EQ(xl::fs::mkdir(_T("d")), true);
  ASSERT_EQ(xl::fs::exists(_T("d")), true);
  ASSERT_EQ(xl::fs::rename(_T("d"), _T("d1")), true);
  ASSERT_EQ(xl::fs::exists(_T("d")), false);
  ASSERT_EQ(xl::fs::exists(_T("d1")), true);
  ASSERT_EQ(xl::fs::mkdirs(_T("d1") XL_FS_SEP _T("d2") XL_FS_SEP _T("d3")), true);
  ASSERT_EQ(xl::fs::exists(_T("d1") XL_FS_SEP _T("d2")), true);
  ASSERT_EQ(xl::fs::exists(_T("d1") XL_FS_SEP _T("d2") XL_FS_SEP _T("d3")), true);
  ASSERT_EQ(xl::fs::rmdir(_T("d1") XL_FS_SEP _T("d2") XL_FS_SEP _T("d3")), true);
  ASSERT_EQ(xl::fs::exists(_T("d1") XL_FS_SEP _T("d2")), true);
  ASSERT_EQ(xl::fs::exists(_T("d1") XL_FS_SEP _T("d2") XL_FS_SEP _T("d3")), false);
  ASSERT_EQ(xl::fs::rmdir(_T("d1") XL_FS_SEP _T("d2")), true);
  ASSERT_EQ(xl::fs::exists(_T("d1")), true);
  ASSERT_EQ(xl::fs::exists(_T("d1") XL_FS_SEP _T("d2")), false);
  ASSERT_EQ(xl::fs::rmdir(_T("d1")), true);
  ASSERT_EQ(xl::fs::exists(_T("d1")), false);
}

TEST(file_test, fs_enum_remove_all) {
  xl::fs::remove_all(_T("d1"));

  ASSERT_EQ(xl::fs::mkdirs(_T("d1") XL_FS_SEP _T("d21") XL_FS_SEP _T("d31")), true);
  ASSERT_EQ(xl::fs::mkdirs(_T("d1") XL_FS_SEP _T("d21") XL_FS_SEP _T("d32")), true);
  ASSERT_EQ(xl::fs::touch(_T("d1") XL_FS_SEP _T("d21") XL_FS_SEP _T("f33")), true);
  ASSERT_EQ(xl::fs::touch(_T("d1") XL_FS_SEP _T("d21") XL_FS_SEP _T("f34")), true);
  ASSERT_EQ(xl::fs::mkdirs(_T("d1") XL_FS_SEP _T("d22") XL_FS_SEP _T("d31")), true);
  ASSERT_EQ(xl::fs::mkdirs(_T("d1") XL_FS_SEP _T("d22") XL_FS_SEP _T("d32")), true);
  ASSERT_EQ(xl::fs::touch(_T("d1") XL_FS_SEP _T("d22") XL_FS_SEP _T("f33")), true);
  ASSERT_EQ(xl::fs::touch(_T("d1") XL_FS_SEP _T("d22") XL_FS_SEP _T("f34")), true);

  std::vector<xl::native_string> dirs;
  auto callback = [&dirs](const xl::native_string &path, bool is_dir) -> bool {
    dirs.push_back(path);
    return true;
  };

  xl::fs::enum_dir(_T("d1"), callback, false, false);
  ASSERT_EQ(std::set<xl::native_string>(dirs.begin(), dirs.end()), (std::set<xl::native_string>{
                                                                       _T("d21"),
                                                                       _T("d22"),
                                                                   }));
  dirs.clear();

  xl::fs::enum_dir(_T("d1") XL_FS_SEP _T("d21"), callback, false, false);
  ASSERT_EQ(std::set<xl::native_string>(dirs.begin(), dirs.end()), (std::set<xl::native_string>{
                                                                       _T("d31"),
                                                                       _T("d32"),
                                                                       _T("f33"),
                                                                       _T("f34"),
                                                                   }));
  dirs.clear();

  xl::fs::enum_dir(_T("d1") XL_FS_SEP _T("d21") XL_FS_SEP _T("d31"), callback, false, false);
  ASSERT_EQ(dirs, (std::vector<xl::native_string>{}));
  dirs.clear();

  xl::fs::enum_dir(_T("d1"), callback, true, false);
  if (dirs[0] == _T("d21")) {
    ASSERT_EQ(std::set<xl::native_string>(dirs.begin() + 1, dirs.begin() + 5), (std::set<xl::native_string>{
                                                                                   _T("d21") XL_FS_SEP _T("d31"),
                                                                                   _T("d21") XL_FS_SEP _T("d32"),
                                                                                   _T("d21") XL_FS_SEP _T("f33"),
                                                                                   _T("d21") XL_FS_SEP _T("f34"),
                                                                               }));
    ASSERT_EQ(dirs[5], _T("d22"));
    ASSERT_EQ(std::set<xl::native_string>(dirs.begin() + 6, dirs.begin() + 10), (std::set<xl::native_string>{
                                                                                    _T("d22") XL_FS_SEP _T("d31"),
                                                                                    _T("d22") XL_FS_SEP _T("d32"),
                                                                                    _T("d22") XL_FS_SEP _T("f33"),
                                                                                    _T("d22") XL_FS_SEP _T("f34"),
                                                                                }));
  } else if (dirs[0] == _T("d22")) {
    ASSERT_EQ(std::set<xl::native_string>(dirs.begin() + 1, dirs.begin() + 5), (std::set<xl::native_string>{
                                                                                   _T("d22") XL_FS_SEP _T("d31"),
                                                                                   _T("d22") XL_FS_SEP _T("d32"),
                                                                                   _T("d22") XL_FS_SEP _T("f33"),
                                                                                   _T("d22") XL_FS_SEP _T("f34"),
                                                                               }));
    ASSERT_EQ(dirs[5], _T("d21"));
    ASSERT_EQ(std::set<xl::native_string>(dirs.begin() + 6, dirs.begin() + 10), (std::set<xl::native_string>{
                                                                                    _T("d21") XL_FS_SEP _T("d31"),
                                                                                    _T("d21") XL_FS_SEP _T("d32"),
                                                                                    _T("d21") XL_FS_SEP _T("f33"),
                                                                                    _T("d21") XL_FS_SEP _T("f34"),
                                                                                }));
  } else {
    FAIL();
  }
  dirs.clear();

  xl::fs::enum_dir(_T("d1"), callback, true, true);
  if (dirs[4] == _T("d21")) {
    ASSERT_EQ(std::set<xl::native_string>(dirs.begin() + 0, dirs.begin() + 4), (std::set<xl::native_string>{
                                                                                   _T("d21") XL_FS_SEP _T("d31"),
                                                                                   _T("d21") XL_FS_SEP _T("d32"),
                                                                                   _T("d21") XL_FS_SEP _T("f33"),
                                                                                   _T("d21") XL_FS_SEP _T("f34"),
                                                                               }));
    ASSERT_EQ(std::set<xl::native_string>(dirs.begin() + 5, dirs.begin() + 9), (std::set<xl::native_string>{
                                                                                   _T("d22") XL_FS_SEP _T("d31"),
                                                                                   _T("d22") XL_FS_SEP _T("d32"),
                                                                                   _T("d22") XL_FS_SEP _T("f33"),
                                                                                   _T("d22") XL_FS_SEP _T("f34"),
                                                                               }));
    ASSERT_EQ(dirs[9], _T("d22"));
  } else if (dirs[4] == _T("d22")) {
    ASSERT_EQ(std::set<xl::native_string>(dirs.begin() + 0, dirs.begin() + 4), (std::set<xl::native_string>{
                                                                                   _T("d22") XL_FS_SEP _T("d31"),
                                                                                   _T("d22") XL_FS_SEP _T("d32"),
                                                                                   _T("d22") XL_FS_SEP _T("f33"),
                                                                                   _T("d22") XL_FS_SEP _T("f34"),
                                                                               }));
    ASSERT_EQ(std::set<xl::native_string>(dirs.begin() + 5, dirs.begin() + 9), (std::set<xl::native_string>{
                                                                                   _T("d21") XL_FS_SEP _T("d31"),
                                                                                   _T("d21") XL_FS_SEP _T("d32"),
                                                                                   _T("d21") XL_FS_SEP _T("f33"),
                                                                                   _T("d21") XL_FS_SEP _T("f34"),
                                                                               }));
    ASSERT_EQ(dirs[9], _T("d21"));
  }
  dirs.clear();

  ASSERT_EQ(xl::fs::remove_all(_T("d1")), true);
  ASSERT_EQ(xl::fs::exists(_T("d1")), false);
}

TEST(file_test, bin_and_utf8) {
  xl::fs::remove(_T("f"));

  ASSERT_EQ(xl::file::write(_T("f"), "\xef\xbb\xbf"
                                     "你好"),
            true);
  ASSERT_EQ(xl::file::read(_T("f")), "\xef\xbb\xbf"
                                     "你好");
  ASSERT_EQ(xl::file::read_text_utf8_bom(_T("f")), "你好");
  ASSERT_EQ(xl::file::read_text_utf16_le(_T("f")), L"");
  ASSERT_EQ(xl::file::read_text_utf16_be(_T("f")), L"");
  ASSERT_EQ(xl::file::read_text_auto(_T("f")), "你好");

  ASSERT_EQ(xl::file::write(_T("f"), "你好"), true);
  ASSERT_EQ(xl::file::read(_T("f")), "你好");
  ASSERT_EQ(xl::file::read_text_utf8_bom(_T("f")), "");
  ASSERT_EQ(xl::file::read_text_utf16_le(_T("f")), L"");
  ASSERT_EQ(xl::file::read_text_utf16_be(_T("f")), L"");
  ASSERT_EQ(xl::file::read_text_auto(_T("f")), "你好");

  ASSERT_EQ(xl::fs::remove(_T("f")), true);
}

TEST(file_test, utf16_le) {
  xl::fs::remove(_T("f"));

  ASSERT_EQ(xl::file::write_text_utf16_le(_T("f"), L"你好"), true); // U+4F60 U+597D
  ASSERT_EQ(xl::file::read(_T("f")), "\xff\xfe\x60\x4f\x7d\x59");
  ASSERT_EQ(xl::file::read_text_utf8_bom(_T("f")), "");
  ASSERT_EQ(xl::file::read_text_utf16_le(_T("f")), L"你好");
  ASSERT_EQ(xl::file::read_text_utf16_be(_T("f")), L"");
  ASSERT_EQ(xl::file::read_text_auto(_T("f")), "你好");
  ASSERT_EQ(xl::fs::remove(_T("f")), true);
}

TEST(file_test, utf16_be) {
  xl::fs::remove(_T("f"));

  ASSERT_EQ(xl::file::write_text_utf16_be(_T("f"), L"你好"), true); // U+4F60 U+597D
  ASSERT_EQ(xl::file::read(_T("f")), "\xfe\xff\x4f\x60\x59\x7d");
  ASSERT_EQ(xl::file::read_text_utf8_bom(_T("f")), "");
  ASSERT_EQ(xl::file::read_text_utf16_le(_T("f")), L"");
  ASSERT_EQ(xl::file::read_text_utf16_be(_T("f")), L"你好");
  ASSERT_EQ(xl::file::read_text_auto(_T("f")), "你好");
  ASSERT_EQ(xl::fs::remove(_T("f")), true);
}
