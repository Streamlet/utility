#pragma once

#include "file.h"
#include "encoding.h"
#include <loki/ScopeGuard.h>

#if defined(_WIN32)
#include <Windows.h>
#endif

#ifdef _MSC_VER

#include <stdlib.h>
#define bswap_16(x) _byteswap_ushort(x)
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)

#elif defined(__APPLE__)

// Mac OS X / Darwin features
#include <libkern/OSByteOrder.h>
#define bswap_16(x) OSSwapInt16(x)
#define bswap_32(x) OSSwapInt32(x)
#define bswap_64(x) OSSwapInt64(x)

#elif defined(__sun) || defined(sun)

#include <sys/byteorder.h>
#define bswap_16(x) BSWAP_16(x)
#define bswap_32(x) BSWAP_32(x)
#define bswap_64(x) BSWAP_64(x)

#elif defined(__FreeBSD__)

#include <sys/endian.h>
#define bswap_16(x) bswap16(x)
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)

#elif defined(__OpenBSD__)

#include <sys/types.h>
#define bswap_16(x) swap16(x)
#define bswap_32(x) swap32(x)
#define bswap_64(x) swap64(x)

#elif defined(__NetBSD__)

#include <machine/bswap.h>
#include <sys/types.h>
#if defined(__BSWAP_RENAME) && !defined(__bswap_32)
#define bswap_16(x) bswap16(x)
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)
#endif

#else

#include <byteswap.h>

#endif

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

bool is_little_endian() {
  return ((char *)&UTF16_BOM)[0] == UTF16_BOM_LE[0];
}

void bytes_swap(std::wstring &content) {
  for (auto &c : content) {
    c = bswap_16(c);
  }
}

std::wstring fread_utf16(FILE *f, size_t size) {
  std::wstring content;
  content.resize(size / sizeof(std::wstring::value_type));
  if (fread(&content[0], size, 1, f) != 1) {
    return L"";
  }
  return std::move(content);
}

std::wstring fread_utf16_le(FILE *f, size_t size) {
  std::wstring content = fread_utf16(f, size);
  if (!is_little_endian()) {
    bytes_swap(content);
  }
  return std::move(content);
}
std::wstring fread_utf16_be(FILE *f, size_t size) {
  std::wstring content = fread_utf16(f, size);
  if (is_little_endian()) {
    bytes_swap(content);
  }
  return std::move(content);
}

bool fwrite_utf16(FILE *f, const std::wstring &text) {
  return fwrite(text.c_str(), text.length() * sizeof(std::wstring::value_type), 1, f) == 1;
}

bool fwrite_utf16_le(FILE *f, const std::wstring &text) {
  if (is_little_endian()) {
    return fwrite_utf16(f, text);
  } else {
    std::wstring copied = text;
    bytes_swap(copied);
    return fwrite_utf16(f, copied);
  }
}

bool fwrite_utf16_be(FILE *f, const std::wstring &text) {
  if (is_little_endian()) {
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
  LOKI_ON_BLOCK_EXIT(fclose, f);
  return true;
}

#define OPEN_CLOSE_FILE_READ(path, errval)                                                                             \
  _tfopen(path, _T("rb"));                                                                                             \
  if (f == nullptr) {                                                                                                  \
    return errval;                                                                                                     \
  }                                                                                                                    \
  LOKI_ON_BLOCK_EXIT(fclose, f)

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
  }
  return "";
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
  LOKI_ON_BLOCK_EXIT(fclose, f)

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
#if defined(_WIN32) && defined(_UNICODE)
  return ::DeleteFileW(path);
#else
  return ::remove(path);
#endif
}

bool rename(const TCHAR *path, const TCHAR *new_path) {
#if defined(_WIN32) && defined(_UNICODE)
  return ::MoveFileW(path, new_path);
#else
  return ::rename(path);
#endif
}

} // namespace file

} // namespace xl
