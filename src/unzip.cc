#include "zip.h"
#include <minizip/unzip.h>
#include <xl/scope_exit>
#include <xl/zip>

#ifdef _WIN32
// clang-format off
#include <zconf.h>
#include <minizip/iowin32.h>
// clang-format on
#endif

namespace xl {

namespace zip {

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

} // namespace zip

} // namespace xl
