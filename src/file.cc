#include <cstring>
#include <xl/byte_order.h>
#include <xl/encoding>
#include <xl/file>
#include <xl/scope_exit>

namespace xl {

namespace file {

namespace {

const char UTF8_BOM[] = {'\xef', '\xbb', '\xbf'};
const wchar_t UTF16_BOM = 0xfeff;
const char UTF16_BOM_LE[] = {'\xff', '\xfe'}, UTF16_BOM_BE[] = {'\xfe', '\xff'};

long long fsize(FILE *f) {
  if (fseek(f, 0, SEEK_END) != 0) {
    return -1;
  }
  return ftell(f);
}

std::string fread(FILE *f, size_t size) {
  std::string content;
  content.resize(size);
  if (fread(&content[0], size, 1, f) != 1) {
    return "";
  }
  return std::move(content);
}

bool fwrite(FILE *f, const std::string &text) {
  return fwrite(text.c_str(), text.length(), 1, f) == 1;
}

constexpr bool is_little_endian() {
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
  return std::move(text);
}

std::wstring fread_utf16_le(FILE *f, size_t size) {
  std::wstring text = fread_utf16(f, size);
  if constexpr (!is_little_endian()) {
    bytes_swap(text);
  }
  return std::move(text);
}

std::wstring fread_utf16_be(FILE *f, size_t size) {
  std::wstring text = fread_utf16(f, size);
  if constexpr (is_little_endian()) {
    bytes_swap(text);
  }
  return std::move(text);
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
  if constexpr (is_little_endian()) {
    return fwrite_utf16(f, text);
  } else {
    std::wstring copied = text;
    bytes_swap(copied);
    return fwrite_utf16(f, copied);
  }
}

bool fwrite_utf16_be(FILE *f, const std::wstring &text) {
  if constexpr (is_little_endian()) {
    std::wstring copied = text;
    bytes_swap(copied);
    return fwrite_utf16(f, copied);
  } else {
    return fwrite_utf16(f, text);
  }
}

} // namespace

bool exists(const TCHAR *path) {
  FILE *f = _tfopen(path, _T("rb"));
  if (f == nullptr) {
    return errno != ENOENT;
  }
  XL_ON_BLOCK_EXIT(fclose, f);
  return true;
}

#define OPEN_CLOSE_FILE_READ(path, errval)                                                                             \
  _tfopen(path, _T("rb"));                                                                                             \
  if (f == nullptr) {                                                                                                  \
    return errval;                                                                                                     \
  }                                                                                                                    \
  XL_ON_BLOCK_EXIT(fclose, f)

long long size(const TCHAR *path) {
  FILE *f = OPEN_CLOSE_FILE_READ(path, -1);
  return fsize(f);
}

std::string read(const TCHAR *path) {
  FILE *f = OPEN_CLOSE_FILE_READ(path, "");
  long long size = fsize(f);
  fseek(f, 0, SEEK_SET);
  return fread(f, size);
}

std::string read_text_auto(const TCHAR *path) {
  FILE *f = OPEN_CLOSE_FILE_READ(path, "");
  long long size = fsize(f);
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
  long long size = fsize(f);
  fseek(f, 0, SEEK_SET);
  char bom[sizeof(UTF8_BOM)] = {};
  if (size < sizeof(bom) || fread(bom, sizeof(bom), 1, f) != 1 || memcmp(bom, UTF8_BOM, sizeof(UTF8_BOM)) != 0) {
    return "";
  }
  return fread(f, size - sizeof(UTF8_BOM));
}

std::wstring read_text_utf16_le(const TCHAR *path) {
  FILE *f = OPEN_CLOSE_FILE_READ(path, L"");
  long long size = fsize(f);
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
  long long size = fsize(f);
  fseek(f, 0, SEEK_SET);
  char bom[sizeof(UTF16_BOM_BE)];
  if (size < sizeof(wchar_t) || fread(&bom, sizeof(bom), 1, f) != 1 ||
      memcmp(bom, UTF16_BOM_BE, sizeof(UTF16_BOM_BE)) != 0) {
    return L"";
  }
  return fread_utf16_be(f, size - sizeof(UTF16_BOM_BE));
}

#define OPEN_CLOSE_FILE_WRITE(path)                                                                                    \
  _tfopen(path, _T("wb"));                                                                                             \
  if (f == nullptr) {                                                                                                  \
    return false;                                                                                                      \
  }                                                                                                                    \
  XL_ON_BLOCK_EXIT(fclose, f)

bool touch(const TCHAR *path) {
  FILE *f = OPEN_CLOSE_FILE_WRITE(path);
  return true;
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

bool remove(const TCHAR *path) {
  return ::_tremove(path) == 0;
}

bool rename(const TCHAR *path, const TCHAR *new_path) {
  return ::_trename(path, new_path) == 0;
}

} // namespace file

} // namespace xl
