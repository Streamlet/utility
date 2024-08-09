#include "zip.h"
#include <cstring>
#include <ctime>
#include <minizip/unzip.h>
#include <string>
#include <xl/encoding>
#include <xl/file>
#include <xl/scope_exit>
#include <xl/string>
#include <xl/zip>
#include <zconf.h>
#ifdef _WIN32
#include <direct.h>
#include <minizip/iowin32.h>
#include <sys/utime.h>
#include <Windows.h>
#else
#include <utime.h>
#endif

namespace xl {

namespace zip {

namespace {

const TCHAR ZIP_SEP = _T('/');
const TCHAR *ZIP_SEP_STR = _T("/");

bool unzip_current_file(unzFile uf, const native_string &target_dir) {
  unz_file_info64 file_info;
  if (unzGetCurrentFileInfo64(uf, &file_info, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK) {
    return false;
  }
  std::string inner_path;
  inner_path.resize(file_info.size_filename);
  if (unzGetCurrentFileInfo64(uf, NULL, &inner_path[0], (uLong)inner_path.size(), NULL, 0, NULL, 0) != UNZ_OK) {
    return false;
  }

#ifdef _WIN32
  native_string inner_path_native;
  if ((file_info.flag & ZIP_GPBF_LANGUAGE_ENCODING_FLAG) != 0) {
#ifdef _UNICODE
    inner_path_native = encoding::utf8_to_utf16(inner_path);
#else
    inner_path_native = encoding::utf16_to_ansi(encoding::utf8_to_utf16(inner_path));
#endif
  } else {
#ifdef _UNICODE
    inner_path_native = encoding::ansi_to_utf16(inner_path);
#else
    inner_path_native = inner_path
#endif
  }
#else
  native_string &inner_path_native = inner_path;
#endif
  native_string outer_path = path::join(target_dir, string::replace(inner_path_native, ZIP_SEP, path::SEP));
  bool is_dir = *inner_path.rbegin() == ZIP_SEP;
  if (is_dir) {
    fs::mkdirs(outer_path.c_str());
  } else {
    fs::mkdirs(path::dirname(outer_path.c_str()).c_str());
    if (unzOpenCurrentFile(uf) != UNZ_OK) {
      return false;
    }
    XL_ON_BLOCK_EXIT(unzCloseCurrentFile, uf);

    FILE *f = _tfopen(outer_path.c_str(), _T("wb"));
    if (f == NULL) {
      return false;
    }
    XL_ON_BLOCK_EXIT(fclose, f);

    const size_t BUFFER_SIZE = 4096;
    unsigned char buffer[BUFFER_SIZE] = {};
    while (true) {
      int size = unzReadCurrentFile(uf, buffer, BUFFER_SIZE);
      if (size < 0) {
        return false;
      }
      if (size == 0) {
        break;
      }
      if (fwrite(buffer, 1, size, f) != size) {
        return false;
      }
    }
  }

  tm date = {};
  date.tm_sec = file_info.tmu_date.tm_sec;
  date.tm_min = file_info.tmu_date.tm_min;
  date.tm_hour = file_info.tmu_date.tm_hour;
  date.tm_mday = file_info.tmu_date.tm_mday;
  date.tm_mon = file_info.tmu_date.tm_mon;
  if (file_info.tmu_date.tm_year > 1900) {
    date.tm_year = file_info.tmu_date.tm_year - 1900;
  } else {
    date.tm_year = file_info.tmu_date.tm_year;
  }
  date.tm_isdst = -1;

#ifdef _WIN32
  struct _utimbuf ut = {};
#else
  struct utimbuf ut = {};
#endif
  ut.actime = ut.modtime = mktime(&date);
  _tutime(outer_path.c_str(), &ut);

  return true;
}

} // namespace

std::string zip_read_file(const TCHAR *zip_file, const char *inner_zip_path) {
#ifdef _WIN32
  zlib_filefunc64_def zlib_filefunc_def;
  fill_win32_filefunc64(&zlib_filefunc_def);
  unzFile uf = unzOpen2_64(zip_file, &zlib_filefunc_def);
#else
  unzFile uf = unzOpen2_64(zip_file, 0);
#endif
  if (uf == NULL) {
    return "";
  }
  XL_ON_BLOCK_EXIT(unzClose, uf);
  if (unzLocateFile(uf, inner_zip_path, 0) != UNZ_OK) {
    return "";
  }
  unz_file_info64 file_info;
  if (unzGetCurrentFileInfo64(uf, &file_info, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK) {
    return "";
  }

  if (unzOpenCurrentFile(uf) != UNZ_OK) {
    return "";
  }
  XL_ON_BLOCK_EXIT(unzCloseCurrentFile, uf);
  std::string content;
  content.resize(file_info.uncompressed_size);
  if (unzReadCurrentFile(uf, &content[0], (unsigned int)content.size()) != content.size()) {
    return "";
  }
  return std::move(content);
}

bool zip_extract(const TCHAR *zip_file, const TCHAR *target_dir) {
#ifdef _WIN32
  zlib_filefunc64_def zlib_filefunc_def;
  fill_win32_filefunc64(&zlib_filefunc_def);
  unzFile uf = unzOpen2_64(zip_file, &zlib_filefunc_def);
#else
  unzFile uf = unzOpen64(zip_file);
#endif
  if (uf == NULL) {
    return false;
  }
  XL_ON_BLOCK_EXIT(unzClose, uf);

  unz_global_info64 gi = {};
  if (unzGetGlobalInfo64(uf, &gi) != UNZ_OK) {
    return false;
  }
  if (gi.number_entry == 0) {
    return true;
  }

  if (unzGoToFirstFile(uf) != UNZ_OK) {
    return false;
  }

  for (int i = 0; i < gi.number_entry; ++i) {
    if (!unzip_current_file(uf, target_dir)) {
      return false;
    }
    if (i < gi.number_entry - 1) {
      if (unzGoToNextFile(uf) != UNZ_OK) {
        return false;
      }
    }
  }

  return true;
}

} // namespace zip

} // namespace xl
