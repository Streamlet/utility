#include "zip.h"
#include <ctime>
#include <minizip/zip.h>
#include <string>
#include <sys/stat.h>
#include <xl/file>
#include <xl/scope_exit>
#include <xl/zip>
#include <zconf.h>
#ifdef _WIN32
#include <io.h>
#include <minizip/iowin32.h>
#include <Windows.h>
#include <xl/encoding>
#endif

namespace xl {

namespace zip {

namespace {

const TCHAR *ZIP_SEP = _T("/");

bool zip_add_item(zipFile zf,
                  const native_string &inner_path,
                  const native_string &outer_path,
                  const fs::stat_data &st) {
  zip_fileinfo file_info = {};
  file_info.internal_fa = 0;
  file_info.external_fa = st.st_mode;
  tm *date = localtime(&st.st_mtime);
  file_info.tmz_date.tm_sec = date->tm_sec;
  file_info.tmz_date.tm_min = date->tm_min;
  file_info.tmz_date.tm_hour = date->tm_hour;
  file_info.tmz_date.tm_mday = date->tm_mday;
  file_info.tmz_date.tm_mon = date->tm_mon;
  file_info.tmz_date.tm_year = date->tm_year;

#ifdef _WIN32
#ifdef _UNICODE
  const std::string inner_path_utf8 = encoding::utf16_to_utf8(inner_path);
#else
  const std::string inner_path_utf8 = encoding::utf16_to_utf8(encoding::ansi_to_utf16(inner_path));
#endif
#else
  const std::string &inner_path_utf8 = inner_path;
#endif
  if (zipOpenNewFileInZip4(zf, inner_path_utf8.c_str(), &file_info, NULL, 0, NULL, 0, NULL, Z_DEFLATED, 9, 0,
                           -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, NULL, 0, 0,
                           ZIP_GPBF_LANGUAGE_ENCODING_FLAG) != ZIP_OK) {
    return false;
  }
  XL_ON_BLOCK_EXIT(zipCloseFileInZip, zf);

  if ((st.st_mode & S_IFMT) == S_IFDIR) {
    return true;
  }

  FILE *f = _tfopen(outer_path.c_str(), _T("rb"));
  if (f == NULL) {
    return false;
  }
  XL_ON_BLOCK_EXIT(fclose, f);

  const unsigned int BUFFER_SIZE = 4096;
  unsigned char buffer[BUFFER_SIZE];
  while (!feof(f)) {
    size_t size = fread(buffer, 1, BUFFER_SIZE, f);
    if (size < BUFFER_SIZE && ferror(f)) {
      return false;
    }
    if (zipWriteInFileInZip(zf, buffer, (unsigned int)size) < 0) {
      return false;
    }
  }
  return true;
}

bool zip_add_file(zipFile zf, const native_string &inner_parent_path, const native_string &outer_path) {
  fs::stat_data st;
  if (!fs::stat(outer_path.c_str(), &st)) {
    return false;
  }
  native_string inner_name = path::filename(outer_path.c_str());
  const native_string &inner_path = (inner_parent_path.empty() ? _T("") : inner_parent_path + ZIP_SEP) + inner_name;
  if (!zip_add_item(zf, inner_path + ((st.st_mode & S_IFMT) == S_IFDIR ? ZIP_SEP : _T("")), outer_path, st)) {
    return false;
  }
  if ((st.st_mode & S_IFMT) != S_IFDIR) {
    return true;
  }
  return xl::fs::enum_dir(outer_path.c_str(),
                          [zf, inner_path, outer_path](const native_string &path, bool is_dir) -> bool {
                            return zip_add_file(zf, inner_path, xl::path::join(outer_path, path));
                          });
}

} // namespace

bool zip_compress(const TCHAR *zip_file, const TCHAR *pattern) {
#ifdef _WIN32
  zlib_filefunc64_def zlib_filefunc_def;
  fill_win32_filefunc64(&zlib_filefunc_def);
  zipFile zf = zipOpen2_64(zip_file, 0, NULL, &zlib_filefunc_def);
#else
  zipFile zf = zipOpen64(zip_file, 0);
#endif
  if (zf == NULL) {
    return false;
  }
  XL_ON_BLOCK_EXIT(zipClose, zf, (const char *)NULL);

  return zip_add_file(zf, _T(""), pattern);
}

} // namespace zip

} // namespace xl
