#include <cstring>
#include <queue>
#include <sys/stat.h>
#include <xl/byte_order.h>
#include <xl/encoding>
#include <xl/file>
#include <xl/scope_exit>

#ifdef _WIN32
#include <io.h>
#else
#include <dirent.h>
#endif
#ifdef __APPLE__
#include <sys/errno.h>
#endif

namespace xl {

namespace {

long long fsize(FILE *f) {
  if (fseek(f, 0, SEEK_END) != 0) {
    return -1;
  }
  return ftell(f);
}

} // namespace

namespace file {

namespace {

const char UTF8_BOM[] = {'\xef', '\xbb', '\xbf'};
const wchar_t UTF16_BOM = 0xfeff;
const char UTF16_BOM_LE[] = {'\xff', '\xfe'}, UTF16_BOM_BE[] = {'\xfe', '\xff'};

std::string fread(FILE *f, size_t size) {
  std::string content;
  content.resize(size);
  if (fread(&content[0], size, 1, f) != 1) {
    return "";
  }
  return content;
}

bool fwrite(FILE *f, const std::string &text) {
  return fwrite(text.c_str(), text.length(), 1, f) == 1;
}

#if __c_plus_plus__ > 201703L
constexpr
#endif
    bool
    is_little_endian() {
#ifdef __BYTE_ORDER__
  return __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__;
#else
  return '\x04\x03\x02\x01' == 0x01020304;
#endif
}

void bytes_swap(std::wstring &content) {
#ifdef _MSC_VER
  for (auto &c : content) {
    c = bswap_16(c);
  }
#else
  for (auto &c : content) {
    *((short *)&c) = bswap_16(*((short *)&c));
    *((short *)&c + 1) = bswap_16(*((short *)&c + 1));
  }
#endif
}

std::wstring fread_utf16(FILE *f, size_t size) {
  std::wstring text;
  text.resize(size / 2);
#ifdef _MSC_VER
  if (fread(&text[0], text.length() * 2, 1, f) != 1) {
    return L"";
  }
#else
  std::basic_string<short> u16string;
  u16string.resize(text.length());
  if (fread(&u16string[0], u16string.length() * 2, 1, f) != 1) {
    return L"";
  }
  for (size_t i = 0; i < text.length(); ++i) {
    text[i] = u16string[i];
  }
#endif
  return text;
}

std::wstring fread_utf16_le(FILE *f, size_t size) {
  std::wstring text = fread_utf16(f, size);
  if
#if __c_plus_plus__ > 201703L
      constexpr
#endif
      (!is_little_endian()) {
    bytes_swap(text);
  }
  return text;
}

std::wstring fread_utf16_be(FILE *f, size_t size) {
  std::wstring text = fread_utf16(f, size);
  if
#if __c_plus_plus__ > 201703L
      constexpr
#endif
      (is_little_endian()) {
    bytes_swap(text);
  }
  return text;
}

bool fwrite_utf16(FILE *f, const std::wstring &text) {
#ifdef _MSC_VER
  return fwrite(text.c_str(), text.length() * 2, 1, f) == 1;
#else
  std::basic_string<short> u16string;
  u16string.resize(text.size());
  for (size_t i = 0; i < text.size(); ++i) {
    u16string[i] = (short)text[i];
  }
  return fwrite(u16string.c_str(), u16string.length() * 2, 1, f) == 1;
#endif
}

bool fwrite_utf16_le(FILE *f, const std::wstring &text) {
  if
#if __c_plus_plus__ > 201703L
      constexpr
#endif
      (is_little_endian()) {
    return fwrite_utf16(f, text);
  } else {
    std::wstring copied = text;
    bytes_swap(copied);
    return fwrite_utf16(f, copied);
  }
}

bool fwrite_utf16_be(FILE *f, const std::wstring &text) {
  if
#if __c_plus_plus__ > 201703L
      constexpr
#endif
      (is_little_endian()) {
    std::wstring copied = text;
    bytes_swap(copied);
    return fwrite_utf16(f, copied);
  } else {
    return fwrite_utf16(f, text);
  }
}

} // namespace

#define OPEN_CLOSE_FILE_READ(path, errval)                                                                             \
  _tfopen(path, _T("rb"));                                                                                             \
  if (f == nullptr) {                                                                                                  \
    return errval;                                                                                                     \
  }                                                                                                                    \
  XL_ON_BLOCK_EXIT(fclose, f)

#define OPEN_CLOSE_FILE_WRITE(path)                                                                                    \
  _tfopen(path, _T("wb"));                                                                                             \
  if (f == nullptr) {                                                                                                  \
    return false;                                                                                                      \
  }                                                                                                                    \
  XL_ON_BLOCK_EXIT(fclose, f)

std::string read(const TCHAR *path) {
  FILE *f = OPEN_CLOSE_FILE_READ(path, "");
  size_t size = (size_t)fsize(f);
  fseek(f, 0, SEEK_SET);
  return fread(f, size);
}

std::string read_text_auto(const TCHAR *path) {
  FILE *f = OPEN_CLOSE_FILE_READ(path, "");
  size_t size = (size_t)fsize(f);
  fseek(f, 0, SEEK_SET);
#pragma pack(push, 1)
  union {
    char utf8[sizeof(UTF8_BOM)];
    char utf16[sizeof(UTF16_BOM_LE)];
  } bom = {};
#pragma pack(pop)
  size_t bom_size = fread(&bom, 1, sizeof(bom), f);
  if (bom_size >= sizeof(UTF8_BOM) && memcmp(bom.utf8, UTF8_BOM, sizeof(UTF8_BOM)) == 0) {
    return fread(f, size - sizeof(UTF8_BOM));
  } else if (bom_size >= sizeof(UTF16_BOM_LE) && memcmp(bom.utf16, UTF16_BOM_LE, sizeof(UTF16_BOM_LE)) == 0) {
    fseek(f, -1, SEEK_CUR);
    std::wstring utf16 = fread_utf16_le(f, size - sizeof(UTF16_BOM_LE));
    return encoding::utf16_to_utf8(utf16);
  } else if (bom_size >= sizeof(UTF16_BOM_BE) && memcmp(bom.utf16, UTF16_BOM_BE, sizeof(UTF16_BOM_BE)) == 0) {
    fseek(f, -1, SEEK_CUR);
    std::wstring utf16 = fread_utf16_be(f, size - sizeof(UTF16_BOM_BE));
    return encoding::utf16_to_utf8(utf16);
  } else {
    fseek(f, -3, SEEK_CUR);
    return fread(f, size);
  }
}

std::string read_text_utf8_bom(const TCHAR *path) {
  FILE *f = OPEN_CLOSE_FILE_READ(path, "");
  size_t size = (size_t)fsize(f);
  fseek(f, 0, SEEK_SET);
  char bom[sizeof(UTF8_BOM)] = {};
  if (size < sizeof(bom) || fread(bom, sizeof(bom), 1, f) != 1 || memcmp(bom, UTF8_BOM, sizeof(UTF8_BOM)) != 0) {
    return "";
  }
  return fread(f, size - sizeof(UTF8_BOM));
}

std::wstring read_text_utf16_le(const TCHAR *path) {
  FILE *f = OPEN_CLOSE_FILE_READ(path, L"");
  size_t size = (size_t)fsize(f);
  fseek(f, 0, SEEK_SET);
  char bom[sizeof(UTF16_BOM_LE)];
  if (size < sizeof(wchar_t) || fread(&bom, sizeof(bom), 1, f) != 1 ||
      memcmp(bom, UTF16_BOM_LE, sizeof(UTF16_BOM_LE)) != 0) {
    return L"";
  }
  return fread_utf16_le(f, size - sizeof(UTF16_BOM_LE));
}

std::wstring read_text_utf16_be(const TCHAR *path) {
  FILE *f = OPEN_CLOSE_FILE_READ(path, L"");
  size_t size = (size_t)fsize(f);
  fseek(f, 0, SEEK_SET);
  char bom[sizeof(UTF16_BOM_BE)];
  if (size < sizeof(wchar_t) || fread(&bom, sizeof(bom), 1, f) != 1 ||
      memcmp(bom, UTF16_BOM_BE, sizeof(UTF16_BOM_BE)) != 0) {
    return L"";
  }
  return fread_utf16_be(f, size - sizeof(UTF16_BOM_BE));
}

bool write(const TCHAR *path, const std::string &text) {
  FILE *f = OPEN_CLOSE_FILE_WRITE(path);
  return fwrite(f, text);
}

bool write_text_uft8_bom(const TCHAR *path, const std::string &text) {
  FILE *f = OPEN_CLOSE_FILE_WRITE(path);
  if (fwrite(UTF8_BOM, sizeof(UTF8_BOM), 1, f) != 1) {
    return false;
  }
  return fwrite(f, text);
}

bool write_text_utf16_le(const TCHAR *path, const std::wstring &text) {
  FILE *f = OPEN_CLOSE_FILE_WRITE(path);
  if (fwrite(&UTF16_BOM_LE, sizeof(UTF16_BOM_LE), 1, f) != 1) {
    return false;
  }
  return fwrite_utf16_le(f, text);
}

bool write_text_utf16_be(const TCHAR *path, const std::wstring &text) {
  FILE *f = OPEN_CLOSE_FILE_WRITE(path);
  if (fwrite(&UTF16_BOM_BE, sizeof(UTF16_BOM_BE), 1, f) != 1) {
    return false;
  }
  return fwrite_utf16_be(f, text);
}

} // namespace file

namespace fs {

bool touch(const TCHAR *path) {
  FILE *f = OPEN_CLOSE_FILE_WRITE(path);
  return true;
}

bool exists(const TCHAR *path) {
  FILE *f = _tfopen(path, _T("rb"));
  if (f == nullptr) {
    return errno != ENOENT;
  }
  XL_ON_BLOCK_EXIT(fclose, f);
  return true;
}

bool stat(const TCHAR *path, stat_data *st) {
#if defined(__APPLE__)
  return _tstat(path, st) == 0;
#else
  return _tstat64(path, st) == 0;
#endif
}

long long size(const TCHAR *path) {
  stat_data st = {};
  if (!fs::stat(path, &st)) {
    return -1;
  }
  return st.st_size;
}

unsigned short attribute(const TCHAR *path) {
  stat_data st = {};
  if (!fs::stat(path, &st)) {
    return 0;
  }
  return st.st_mode;
}

bool is_dir(const TCHAR *path) {
  return (attribute(path) & S_IFMT) == S_IFDIR;
}

bool unlink(const TCHAR *path) {
  return ::_tunlink(path) == 0;
}

bool mkdir(const TCHAR *path) {
#ifdef _WIN32
  return ::_tmkdir(path) == 0;
#else
  return ::_tmkdir(path, 0755) == 0;
#endif
}

bool mkdirs(const TCHAR *path) {
  native_string paths = path;
  for (TCHAR *p = _tcschr(&paths[0], path::SEP); p != NULL; p = _tcschr(p + 1, path::SEP)) {
    *p = _T('\0');
    fs::mkdir(paths.c_str());
    *p = path::SEP;
  }
  if (!fs::mkdir(paths.c_str())) {
    return false;
  }
  return true;
}

namespace {

bool enum_dir(const native_string &path,
              EnumDirCallback callback,
              bool recursive,
              bool sub_dir_first,
              const native_string &found_path_prefix) {
#ifdef _WIN32
  native_string pattern = path + path::SEP_STR + _T("*");
  _wfinddata64_t find_data = {};
  intptr_t find = _wfindfirst64(pattern.c_str(), &find_data);
  if (find == -1) {
    return false;
  }
  XL_ON_BLOCK_EXIT(_findclose, find);

  do {
    if (_tcscmp(find_data.name, _T(".")) == 0 || _tcscmp(find_data.name, _T("..")) == 0) {
      continue;
    }
    native_string absolute_path = path + path::SEP_STR + find_data.name;
    native_string found_path = found_path_prefix + find_data.name;
    bool is_dir = (find_data.attrib & _A_SUBDIR) != 0;
#else
  DIR *dir = opendir(path.c_str());
  if (dir == NULL) {
    return false;
  }
  XL_ON_BLOCK_EXIT(closedir, dir);
  for (dirent *item = readdir(dir); item != nullptr; item = readdir(dir)) {
    if (_tcscmp(item->d_name, _T(".")) == 0 || _tcscmp(item->d_name, _T("..")) == 0) {
      continue;
    }

    native_string absolute_path = path + path::SEP_STR + item->d_name;
    native_string found_path = found_path_prefix + item->d_name;
    bool is_dir = (item->d_type & DT_DIR) != 0;
#endif

    if (!is_dir || !recursive || !sub_dir_first) {
      if (!callback(found_path, is_dir)) {
        return false;
      }
    }
    if (is_dir && recursive) {
      if (!enum_dir(absolute_path, callback, recursive, sub_dir_first, found_path + path::SEP_STR)) {
        return false;
      }
    }
    if (is_dir && recursive && sub_dir_first) {
      if (!callback(found_path, is_dir)) {
        return false;
      }
    }

#ifdef _WIN32
  } while (_wfindnext64(find, &find_data) == 0);
#else
  }
#endif
  return true;
}

} // namespace

bool enum_dir(const TCHAR *path, EnumDirCallback callback, bool recursive, bool sub_dir_first) {
  return enum_dir(path, callback, recursive, sub_dir_first, _T(""));
}

bool rmdir(const TCHAR *path) {
  return ::_trmdir(path) == 0;
}

bool remove(const TCHAR *path) {
  return ::_tremove(path) == 0;
}

bool remove_all(const TCHAR *path) {
  if (!enum_dir(
          path,
          [path](const native_string &sub_path, bool is_dir) -> bool {
            native_string p = path + path::SEP_STR + sub_path;
            if (is_dir) {
              return rmdir(p.c_str());
            } else {
              return unlink(p.c_str());
            }
          },
          true, true)) {
    return false;
  }
  return rmdir(path);
}

bool rename(const TCHAR *path, const TCHAR *new_path) {
  return ::_trename(path, new_path) == 0;
}

} // namespace fs

namespace path {

#ifdef _WIN32
const TCHAR SEP = _T('\\');
const native_string SEP_STR = _T("\\");
#else
const TCHAR SEP = _T('/');
const native_string SEP_STR = _T("/");
#endif

const TCHAR DOT = _T('.');
const native_string DOT_STR = _T(".");

native_string dirname(const TCHAR *path) {
  const TCHAR *sep_pos = _tcsrchr(path, path::SEP);
  if (sep_pos == nullptr) {
    return _T("");
  }
  if (sep_pos == path) {
    return SEP_STR;
  }
  return native_string(path, sep_pos - path);
}

native_string filename(const TCHAR *path) {
  const TCHAR *name_pos = _tcsrchr(path, path::SEP);
  if (name_pos == nullptr) {
    name_pos = path;
  } else {
    ++name_pos;
  }
  return native_string(name_pos);
}

native_string basename(const TCHAR *path) {
  const TCHAR *name_pos = _tcsrchr(path, path::SEP);
  if (name_pos == nullptr) {
    name_pos = path;
  } else {
    ++name_pos;
  }
  const TCHAR *dot_pos = _tcsrchr(name_pos, path::DOT);
  if (dot_pos == nullptr) {
    return name_pos;
  }
  return native_string(name_pos, dot_pos - name_pos);
}

native_string extname(const TCHAR *path) {
  const TCHAR *name_pos = _tcsrchr(path, path::SEP);
  if (name_pos == nullptr) {
    name_pos = path;
  } else {
    ++name_pos;
  }
  const TCHAR *dot_pos = _tcsrchr(name_pos, path::DOT);
  if (dot_pos == nullptr) {
    return _T("");
  }
  return native_string(dot_pos);
}

} // namespace path

} // namespace xl
