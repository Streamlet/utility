#include <cstring>
#include <ctime>
#include <minizip/unzip.h>
#include <minizip/zip.h>
#include <string>
#include <sys/stat.h>
#include <xl/encoding>
#include <xl/file>
#include <xl/scope_exit>
#include <xl/string>
#include <xl/zip>
#include <zconf.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <minizip/iowin32.h>
#include <sys/utime.h>
#include <Windows.h>
#include <xl/encoding>
#else
#include <utime.h>
#endif

#define ZIP_GPBF_LANGUAGE_ENCODING_FLAG 0x800

namespace xl {

namespace zip {

namespace {

const TCHAR ZIP_SEP = _T('/');
const TCHAR *ZIP_SEP_STR = _T("/");

bool zip_extract_current_item(unzFile uf, const native_string &target_dir) {
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
  const native_string &inner_path = (inner_parent_path.empty() ? _T("") : inner_parent_path + ZIP_SEP_STR) + inner_name;
  if (!zip_add_item(zf, inner_path + ((st.st_mode & S_IFMT) == S_IFDIR ? ZIP_SEP_STR : _T("")), outer_path, st)) {
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
    if (!zip_extract_current_item(uf, target_dir)) {
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
