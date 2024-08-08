#include "zip.h"
#include <ctime>
#include <io.h>
#include <minizip/zip.h>
#include <string>
#include <Windows.h>
#include <xl/encoding>
#include <xl/scope_exit>
#include <xl/zip>
// clang-format off
#include <zconf.h>
#include <minizip/iowin32.h>
// clang-format on

namespace xl {

namespace zip {

namespace {

bool zip_add_file(zipFile zf,
                  const native_string &inner_path,
                  const native_string &source_file,
                  const _wfinddata64_t &find_data) {
  zip_fileinfo file_info = {};
  file_info.internal_fa = 0;
  file_info.external_fa = find_data.attrib;
  HANDLE hFile = CreateFile(source_file.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING,
                            (find_data.attrib & _A_SUBDIR) != 0 ? FILE_ATTRIBUTE_DIRECTORY : 0, NULL);
  if (hFile != INVALID_HANDLE_VALUE) {
    FILETIME ftLocal = {}, ftUTC = {};
    GetFileTime(hFile, NULL, NULL, &ftUTC);
    FileTimeToLocalFileTime(&ftUTC, &ftLocal);
    WORD wDate = 0, wTime = 0;
    FileTimeToDosDateTime(&ftLocal, &wDate, &wTime);
    CloseHandle(hFile);
    file_info.dosDate = ((((DWORD)wDate) << 16) | (DWORD)wTime);
  }

#ifdef _UNICODE
  const std::string inner_path_utf8 = encoding::utf16_to_utf8(inner_path);
#else
  const std::string inner_path_utf8 = encoding::utf16_to_utf8(encoding::ansi_to_utf16(inner_path));
#endif
  if (zipOpenNewFileInZip4(zf, inner_path_utf8.c_str(), &file_info, NULL, 0, NULL, 0, NULL, Z_DEFLATED, 9, 0,
                           -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, NULL, 0, 0,
                           ZIP_GPBF_LANGUAGE_ENCODING_FLAG) != ZIP_OK) {
    return false;
  }
  XL_ON_BLOCK_EXIT(zipCloseFileInZip, zf);

  if ((find_data.attrib & _A_SUBDIR) != 0) {
    return true;
  }

  FILE *f = _tfopen(source_file.c_str(), _T("rb"));
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

bool zip_add_files_pattern(zipFile zf, const native_string &inner_dir, const native_string &pattern) {
  size_t slash = pattern.rfind(_T('/'));
  size_t back_slash = pattern.rfind(_T('\\'));
  size_t slash_pos = slash != native_string::npos && back_slash != native_string::npos
                         ? max(slash, back_slash)
                         : (slash != native_string::npos ? slash : back_slash);
  native_string source_dir;
  if (slash_pos != native_string::npos) {
    source_dir = pattern.substr(0, slash_pos) + _T("/");
  }

  _wfinddata64_t find_data = {};
  intptr_t find = _wfindfirst64(pattern.c_str(), &find_data);
  if (find == -1) {
    return false;
  }
  XL_ON_BLOCK_EXIT(_findclose, find);

  do {
    if (wcscmp(find_data.name, _T(".")) == 0 || wcscmp(find_data.name, _T("..")) == 0) {
      continue;
    }

    native_string inner_path = inner_dir + find_data.name;
    native_string source_path = source_dir + find_data.name;
    if ((find_data.attrib & _A_SUBDIR) != 0) {
      inner_path += _T("/");
      if (!zip_add_file(zf, inner_path, source_path, find_data)) {
        return false;
      }
      if (!zip_add_files_pattern(zf, inner_path, source_path + _T("/*"))) {
        return false;
      }
    } else {
      if (!zip_add_file(zf, inner_path, source_path, find_data)) {
        return false;
      }
    }
  } while (_wfindnext64(find, &find_data) == 0);

  return true;
}

} // namespace

bool zip_compress(const TCHAR *zip_file, const TCHAR *pattern) {
  zlib_filefunc64_def zlib_filefunc_def;
  fill_win32_filefunc64(&zlib_filefunc_def);
  zipFile zf = zipOpen2_64(zip_file, 0, NULL, &zlib_filefunc_def);
  if (zf == NULL) {
    return false;
  }
  XL_ON_BLOCK_EXIT(zipClose, zf, (const char *)NULL);

  return zip_add_files_pattern(zf, _T(""), pattern);
}

} // namespace zip

} // namespace xl
