#include <cstring>
#include <ctime>
#include <functional>
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

//
// https://pkwaredownloads.blob.core.windows.net/pem/APPNOTE.txt
// 4.4.2 version made by (2 bytes)
//
#define ZIP_HOST_SYSTEM_MSDOS         0  //  MS-DOS and OS/2 (FAT / VFAT / FAT32 file systems)
#define ZIP_HOST_SYSTEM_AMIGA         1  //  Amiga
#define ZIP_HOST_SYSTEM_OPEN_VMS      2  //  OpenVMS
#define ZIP_HOST_SYSTEM_UNIX          3  //  UNIX
#define ZIP_HOST_SYSTEM_VM_CMS        4  //  VM/CMS
#define ZIP_HOST_SYSTEM_ATARI_ST      5  //  Atari ST
#define ZIP_HOST_SYSTEM_OS_2_HPFS     6  //  OS/2 H.P.F.S.
#define ZIP_HOST_SYSTEM_MACINTOSH     7  //  Macintosh
#define ZIP_HOST_SYSTEM_Z_SYSTEM      8  //  Z-System
#define ZIP_HOST_SYSTEM_CP_M          9  //  CP/M
#define ZIP_HOST_SYSTEM_WINDOWS_NTFS  10 //  Windows NTFS
#define ZIP_HOST_SYSTEM_MVS           11 //  MVS (OS/390 - Z/OS)
#define ZIP_HOST_SYSTEM_VSE           12 //  VSE
#define ZIP_HOST_SYSTEM_ACRON_RISC    13 //  Acorn Risc
#define ZIP_HOST_SYSTEM_VFAT          14 //  VFAT
#define ZIP_HOST_SYSTEM_ALTERNATE_MVS 15 //  alternate MVS
#define ZIP_HOST_SYSTEM_BEOS          16 //  BeOS
#define ZIP_HOST_SYSTEM_TANDEM        17 //  Tandem
#define ZIP_HOST_SYSTEM_OS_400        18 //  OS/400
#define ZIP_HOST_SYSTEM_OS_X_DARWIN   19 //  OS X (Darwin)

#define ZIP_SPEC_VERSION 45

#if defined(_WIN32)
// For both MS-DOS(0) AND WINDOWS NTFS(10), there is no difference of external file attribute (FILE_ATTRIBUTE_*).
// We use ZIP_HOST_SYSTEM_MSDOS for more compatibility. (e.g., 7-zip treats value 11 as WINDOWS NTFS)
#define ZIP_CURRENT_HOST_SYSTEM ZIP_HOST_SYSTEM_MSDOS
#else
#define ZIP_CURRENT_HOST_SYSTEM ZIP_HOST_SYSTEM_UNIX
#endif

#define ZIP_VERSION_MADEBY ((ZIP_CURRENT_HOST_SYSTEM << 8) | ZIP_SPEC_VERSION)

#define ZIP_EXTRACT_HOST_SYSTEM(version_made_by) ((unsigned char)((version_made_by) >> 8))

//
// https://pkwaredownloads.blob.core.windows.net/pem/APPNOTE.txt
// 4.4.4 general purpose bit flag: (2 bytes)
//

// Bit 0: If set, indicates that the file is encrypted.
#define ZIP_FLAG_ENCRYPTED (1 << 0)
// (For Method 6 - Imploding)
// Bit 1: If the compression method used was type 6,
//        Imploding, then this bit, if set, indicates
//        an 8K sliding dictionary was used.  If clear,
//        then a 4K sliding dictionary was used.
#define ZIP_FLAG_IMPLODE_8K_SLIDING_DIRECTORY (1 << 1)
// Bit 2: If the compression method used was type 6,
//        Imploding, then this bit, if set, indicates
//        3 Shannon-Fano trees were used to encode the
//        sliding dictionary output.  If clear, then 2
//        Shannon-Fano trees were used.
#define ZIP_FLAG_IMPLODE_3_SHANNON_FANO_TREES (1 << 2)
// (For Methods 8 and 9 - Deflating)
// Bit 2  Bit 1
//   0      0    Normal (-en) compression option was used.
//   0      1    Maximum (-exx/-ex) compression option was used.
//   1      0    Fast (-ef) compression option was used.
//   1      1    Super Fast (-es) compression option was used.
#define ZIP_FLAG_DEFLATE_NORMAL     0
#define ZIP_FLAG_DEFLATE_MAXIMUM    (1 << 1)
#define ZIP_FLAG_DEFLATE_FAST       (1 << 2)
#define ZIP_FLAG_DEFLATE_SUPER_FAST (ZIP_FLAG_DEFLATE_MAXIMUM | ZIP_FLAG_DEFLATE_FAST)
#define ZIP_FLAG_DEFLATE_MASK       ZIP_FLAG_DEFLATE_SUPER_FAST
// (For Method 14 - LZMA)
// Bit 1: If the compression method used was type 14,
//        LZMA, then this bit, if set, indicates
//        an end-of-stream (EOS) marker is used to
//        mark the end of the compressed data stream.
//        If clear, then an EOS marker is not present
//        and the compressed data size must be known
//        to extract.
#define ZIP_FLAG_LZMA_EOS_STREAMER (1 << 1)
// Note:  Bits 1 and 2 are undefined if the compression
//        method is any other.
// Bit 3: If this bit is set, the fields crc-32, compressed
//        size and uncompressed size are set to zero in the
//        local header.  The correct values are put in the
//        data descriptor immediately following the compressed
//        data.  (Note: PKZIP version 2.04g for DOS only
//        recognizes this bit for method 8 compression, newer
//        versions of PKZIP recognize this bit for any
//        compression method.)
#define ZIP_FLAG_DATA_DESCRIPTOR (1 << 3)
// Bit 4: Reserved for use with method 8, for enhanced
//        deflating.
// Bit 5: If this bit is set, this indicates that the file is
//        compressed patched data.  (Note: Requires PKZIP
//        version 2.70 or greater)
#define ZIP_FLAG_COMPRESSED_PATCHED_DATA (1 << 5)
// Bit 6: Strong encryption.  If this bit is set, you MUST
//        set the version needed to extract value to at least
//        50 and you MUST also set bit 0.  If AES encryption
//        is used, the version needed to extract value MUST
//        be at least 51. See the section describing the Strong
//        Encryption Specification for details.  Refer to the
//        section in this document entitled "Incorporating PKWARE
//        Proprietary Technology into Your Product" for more
//        information.
#define ZIP_FLAG_STRONG_ENCRYPT (1 << 6)
// Bit 7: Currently unused.
// Bit 8: Currently unused.
// Bit 9: Currently unused.
// Bit 10: Currently unused.
// Bit 11: Language encoding flag (EFS).  If this bit is set,
//         the filename and comment fields for this file
//         MUST be encoded using UTF-8. (see APPENDIX D)
#define ZIP_FLAG_UTF8 (1 << 11)
// Bit 12: Reserved by PKWARE for enhanced compression.
// Bit 13: Set when encrypting the Central Directory to indicate
//         selected data values in the Local Header are masked to
//         hide their actual values.  See the section describing
//         the Strong Encryption Specification for details.  Refer
//         to the section in this document entitled "Incorporating
//         PKWARE Proprietary Technology into Your Product" for
//         more information.
#define ZIP_FLAG_MASK_LOCAL_INFO (1 << 13)
// Bit 14: Reserved by PKWARE for alternate streams.
// Bit 15: Reserved by PKWARE.

namespace xl {

namespace zip {

////////////////////////////////////////////////////////////////////////////////
// common utilities

namespace {

const char ZIP_SEP = '/';
const char *ZIP_SEP_STR = "/";
const TCHAR ZIP_SEP_NATIVE = _T('/');

// re-arranged from minizip-ng: mz_zip_attrib_posix_to_win32
unsigned int posix_attrobute_to_win32(unsigned int posix_st_mode) {
  unsigned int win32_file_attribute = 0;
  /* S_IWUSR | S_IWGRP | S_IWOTH | S_IXUSR | S_IXGRP | S_IXOTH */
  if ((posix_st_mode & 0000333) == 0 && (posix_st_mode & 0000444) != 0) {
    win32_file_attribute |= 0x01; /* FILE_ATTRIBUTE_READONLY */
  }
  /* S_IFLNK */
  if ((posix_st_mode & 0170000) == 0120000) {
    win32_file_attribute |= 0x400; /* FILE_ATTRIBUTE_REPARSE_POINT */
  }
  /* S_IFDIR */
  else if ((posix_st_mode & 0170000) == 0040000) {
    win32_file_attribute |= 0x10; /* FILE_ATTRIBUTE_DIRECTORY */
  }
  /* S_IFREG */
  else {
    win32_file_attribute |= 0x80; /* FILE_ATTRIBUTE_NORMAL */
  }
  return win32_file_attribute;
}

// re-arranged from minizip-ng: mz_zip_attrib_win32_to_posix
unsigned int win32_attrobute_to_posix(unsigned int win32_file_attribute) {
  unsigned int posix_st_mode = 0000444; /* S_IRUSR | S_IRGRP | S_IROTH */
  /* FILE_ATTRIBUTE_READONLY */
  if ((win32_file_attribute & 0x01) == 0) {
    posix_st_mode |= 0000222; /* S_IWUSR | S_IWGRP | S_IWOTH */
  }
  /* FILE_ATTRIBUTE_REPARSE_POINT */
  if ((win32_file_attribute & 0x400) == 0x400) {
    posix_st_mode |= 0120000; /* S_IFLNK */
  }
  /* FILE_ATTRIBUTE_DIRECTORY */
  else if ((win32_file_attribute & 0x10) == 0x10) {
    posix_st_mode |= 0040111; /* S_IFDIR | S_IXUSR | S_IXGRP | S_IXOTH */
  } else {
    posix_st_mode |= 0100000; /* S_IFREG */
  }
  return posix_st_mode;
}

// re-arranged from minizip-ng: mz_zip_attrib_convert
unsigned int external_fa_to_attribute(unsigned short version_made_by, unsigned int external_fa) {
  unsigned char file_made_by_system = ZIP_EXTRACT_HOST_SYSTEM(version_made_by);
  switch (file_made_by_system) {
  case ZIP_HOST_SYSTEM_MSDOS:
  case ZIP_HOST_SYSTEM_WINDOWS_NTFS:
#ifdef _WIN32
    return external_fa;
#else
    return win32_attrobute_to_posix(external_fa);
#endif
  case ZIP_HOST_SYSTEM_UNIX:
  case ZIP_HOST_SYSTEM_OS_X_DARWIN:
  case ZIP_HOST_SYSTEM_ACRON_RISC:
  default:
    /* If high bytes are set, it contains unix specific attributes */
    if ((external_fa >> 16) != 0) {
      external_fa >>= 16;
    }
#ifdef _WIN32
    return posix_attrobute_to_win32(external_fa);
#else
    return external_fa;
#endif
  }
}

bool is_entry_dir(const std::string &inner_path, unsigned int attribute) {
  if (fs::is_dir(attribute)) {
    return true;
  }
  if (!inner_path.empty() && (inner_path.back() == '/' || inner_path.back() == '\\')) {
    return true;
  }
  return false;
}

time_t tm_date_to_time(const tm_unz &tm_date) {
  tm tm = {};
  tm.tm_sec = tm_date.tm_sec;
  tm.tm_min = tm_date.tm_min;
  tm.tm_hour = tm_date.tm_hour;
  tm.tm_mday = tm_date.tm_mday;
  tm.tm_mon = tm_date.tm_mon;
  if (tm_date.tm_year > 1900) {
    tm.tm_year = tm_date.tm_year - 1900;
  } else {
    tm.tm_year = tm_date.tm_year;
  }
  tm.tm_isdst = -1;
  return mktime(&tm);
}

void time_to_tm_date(time_t time, tm_zip &tm_date) {
  tm *date = localtime(&time);
  tm_date.tm_sec = date->tm_sec;
  tm_date.tm_min = date->tm_min;
  tm_date.tm_hour = date->tm_hour;
  tm_date.tm_mday = date->tm_mday;
  tm_date.tm_mon = date->tm_mon;
  tm_date.tm_year = date->tm_year;
}

void make_file_info(unsigned int attribute, time_t modify_time, zip_fileinfo &file_info) {
  file_info.internal_fa = 0;
  file_info.external_fa = attribute;
  time_to_tm_date(modify_time, file_info.tmz_date);
}

std::string inner_path_to_utf8(const native_string &inner_path) {
#ifdef _WIN32
#ifdef _UNICODE
  return encoding::utf16_to_utf8(inner_path);
#else
  return encoding::utf16_to_utf8(encoding::ansi_to_utf16(inner_path));
#endif
#else
  return inner_path;
#endif
}

native_string inner_path_to_native(const std::string &inner_path, unsigned short flag) {
#ifdef _WIN32
  native_string inner_path_native;
  if ((flag & ZIP_FLAG_UTF8) != 0) {
#ifdef _UNICODE
    return encoding::utf8_to_utf16(inner_path);
#else
    return encoding::utf16_to_ansi(encoding::utf8_to_utf16(inner_path));
#endif
  } else {
#ifdef _UNICODE
    return encoding::ansi_to_utf16(inner_path);
#else
    return inner_path
#endif
  }
#else
  return inner_path;
#endif
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
// compressing internal

namespace {

bool zip_new_inner_file(zipFile zf, const std::string &inner_path, unsigned int attribute, time_t modify_time) {
  zip_fileinfo file_info = {};
  make_file_info(attribute, modify_time, file_info);
  if (zipOpenNewFileInZip4(zf, inner_path.c_str(), &file_info, NULL, 0, NULL, 0, NULL, Z_DEFLATED, 9, 0, -MAX_WBITS,
                           DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, NULL, 0, ZIP_VERSION_MADEBY, ZIP_FLAG_UTF8) != ZIP_OK) {
    return false;
  }
  return true;
}

void zip_close_inner_file(zipFile zf) {
  zipCloseFileInZip(zf);
}

bool zip_add_entry(zipFile zf,
                   const std::string &inner_path,
                   const void *data,
                   size_t length,
                   unsigned int attribute,
                   time_t modify_time) {
  if (!zip_new_inner_file(zf, inner_path, attribute, modify_time)) {
    return false;
  }
  XL_ON_BLOCK_EXIT(zip_close_inner_file, zf);
  if (fs::is_dir(attribute)) {
    return true;
  }
  if (zipWriteInFileInZip(zf, data, (unsigned int)length) < 0) {
    return false;
  }
  return true;
}

bool zip_add_entry(zipFile zf,
                   const std::string &inner_path,
                   const native_string &outer_path,
                   unsigned int attribute,
                   time_t modify_time) {
  if (!zip_new_inner_file(zf, inner_path, attribute, modify_time)) {
    return false;
  }
  XL_ON_BLOCK_EXIT(zip_close_inner_file, zf);
  if (fs::is_dir(attribute)) {
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

bool zip_add_file_or_dir(zipFile zf, const std::string &inner_path, const native_string &outer_path) {
  fs::stat_data st;
  if (!fs::stat(outer_path.c_str(), &st)) {
    return false;
  }
#ifdef _WIN32
  unsigned int attribute = fs::attribute(outer_path.c_str());
#else
  unsigned int attribute = st.st_mode;
#endif
  time_t modify_time = st.st_mtime;
  if (!zip_add_entry(zf, inner_path, outer_path, attribute, modify_time)) {
    return false;
  }
  if (!fs::is_dir(attribute)) {
    return true;
  }
  return fs::enum_dir(outer_path.c_str(), [zf, inner_path, outer_path](const native_string &path, bool is_dir) -> bool {
    const std::string sub_inner_file =
        (inner_path.empty() ? "" : inner_path + ZIP_SEP_STR) + string::replace(inner_path_to_utf8(path), '\\', ZIP_SEP);
    const native_string sub_outer_file = path::join(outer_path, path);
    return zip_add_file_or_dir(zf, sub_inner_file, sub_outer_file);
  });
}

zipFile zip_open_file(const TCHAR *zip_file, bool force_create = false) {
  int append =
      force_create ? APPEND_STATUS_CREATE : (fs::exists(zip_file) ? APPEND_STATUS_ADDINZIP : APPEND_STATUS_CREATE);
#ifdef _WIN32
  zlib_filefunc64_def zlib_filefunc_def;
  fill_win32_filefunc64(&zlib_filefunc_def);
  return zipOpen2_64(zip_file, append, NULL, &zlib_filefunc_def);
#else
  return zipOpen64(zip_file, append);
#endif
}

void zip_close_file(zipFile zf) {
  zipClose(zf, (const char *)NULL);
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
// extracting internal

namespace {

typedef std::function<size_t(const void *data, size_t length)> UnzipExtractDataCallback;

bool unzip_extract_current_entry_data(unzFile uf, UnzipExtractDataCallback callback) {
  if (unzOpenCurrentFile(uf) != UNZ_OK) {
    return false;
  }
  XL_ON_BLOCK_EXIT(unzCloseCurrentFile, uf);

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
    if (callback(buffer, size) != size) {
      return false;
    }
  }
  return true;
}

bool unzip_extract_current_entry_data(unzFile uf, std::string &content) {
  return unzip_extract_current_entry_data(uf, [&content](const void *data, size_t length) -> size_t {
    content.append((const char *)data, length);
    return length;
  });
}

bool unzip_extract_current_entry_data(unzFile uf, const TCHAR *outer_path) {
  FILE *f = _tfopen(outer_path, _T("wb"));
  if (f == NULL) {
    return false;
  }
  XL_ON_BLOCK_EXIT(fclose, f);
  return unzip_extract_current_entry_data(uf, [&f](const void *data, size_t length) -> size_t {
    return fwrite(data, 1, length, f);
  });
}

bool zip_extract_current_entry(unzFile uf, const native_string &target_dir) {
  unz_file_info64 file_info;
  if (unzGetCurrentFileInfo64(uf, &file_info, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK) {
    return false;
  }
  unsigned int attribute = external_fa_to_attribute((unsigned short)file_info.version, file_info.external_fa);
  std::string inner_path;
  inner_path.resize(file_info.size_filename);
  if (unzGetCurrentFileInfo64(uf, NULL, &inner_path[0], (uLong)inner_path.size(), NULL, 0, NULL, 0) != UNZ_OK) {
    return false;
  }
  bool is_dir = is_entry_dir(inner_path, attribute);
  native_string inner_path_native = inner_path_to_native(inner_path, (unsigned short)file_info.flag);

  native_string outer_path = path::join(target_dir, string::replace(inner_path_native, ZIP_SEP_NATIVE, path::SEP));
  if (is_dir) {
    fs::mkdirs(outer_path.c_str());
  } else {
    fs::mkdirs(path::dirname(outer_path.c_str()).c_str());
    if (!unzip_extract_current_entry_data(uf, outer_path.c_str())) {
      return false;
    }
  }

  fs::attribute(outer_path.c_str(), attribute);
#ifdef _WIN32
  struct _utimbuf ut = {};
#else
  struct utimbuf ut = {};
#endif
  ut.actime = ut.modtime = tm_date_to_time(file_info.tmu_date);
  _tutime(outer_path.c_str(), &ut);

  return true;
}

unzFile unzip_open_file(const TCHAR *zip_file) {
#ifdef _WIN32
  zlib_filefunc64_def zlib_filefunc_def;
  fill_win32_filefunc64(&zlib_filefunc_def);
  return unzOpen2_64(zip_file, &zlib_filefunc_def);
#else
  return unzOpen2_64(zip_file, 0);
#endif
}

void unzip_close_file(unzFile uf) {
  unzClose(uf);
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
// compressing

bool touch(const TCHAR *zip_file) {
  zipFile zf = zip_open_file(zip_file);
  if (zf == NULL) {
    return false;
  }
  XL_ON_BLOCK_EXIT(zip_close_file, zf);
  return true;
}

bool add_file(const TCHAR *zip_file, const char *inner_path, const void *data, size_t length) {
  zipFile zf = zip_open_file(zip_file);
  if (zf == NULL) {
    return false;
  }
  XL_ON_BLOCK_EXIT(zip_close_file, zf);
#ifdef _WIN32
  unsigned int attribute = FILE_ATTRIBUTE_NORMAL;
#else
  unsigned int attribute = S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR;
#endif
  return zip_add_entry(zf, inner_path, data, length, attribute, time(NULL));
}

bool add_file(const TCHAR *zip_file, const char *inner_path, const TCHAR *outer_path) {
  zipFile zf = zip_open_file(zip_file);
  if (zf == NULL) {
    return false;
  }
  XL_ON_BLOCK_EXIT(zip_close_file, zf);
  return zip_add_file_or_dir(zf, inner_path, outer_path);
}

bool compress(const TCHAR *zip_file, const TCHAR *outer_path) {
  zipFile zf = zip_open_file(zip_file, true);
  if (zf == NULL) {
    return false;
  }
  XL_ON_BLOCK_EXIT(zip_close_file, zf);
  return zip_add_file_or_dir(zf, inner_path_to_utf8(path::filename(outer_path)), outer_path);
}

bool compress_folder_content(const TCHAR *zip_file, const TCHAR *outer_path) {
  zipFile zf = zip_open_file(zip_file, true);
  if (zf == NULL) {
    return false;
  }
  XL_ON_BLOCK_EXIT(zip_close_file, zf);
  return zip_add_file_or_dir(zf, "", outer_path);
}

////////////////////////////////////////////////////////////////////////////////
// extracting

std::string read_file(const TCHAR *zip_file, const char *inner_path) {
  unzFile uf = unzip_open_file(zip_file);
  if (uf == NULL) {
    return "";
  }
  XL_ON_BLOCK_EXIT(unzip_close_file, uf);
  if (unzLocateFile(uf, inner_path, 0) != UNZ_OK) {
    return "";
  }
  std::string content;
  if (!unzip_extract_current_entry_data(uf, content)) {
    return "";
  }
  return std::move(content);
}

bool extract(const TCHAR *zip_file, const TCHAR *target_dir) {
  unzFile uf = unzip_open_file(zip_file);
  if (uf == NULL) {
    return false;
  }
  XL_ON_BLOCK_EXIT(unzip_close_file, uf);
  unz_global_info64 gi = {};
  if (unzGetGlobalInfo64(uf, &gi) != UNZ_OK) {
    return false;
  }
  if (gi.number_entry == 0) {
    return true;
  }
  int err = unzGoToFirstFile(uf);
  while (err == UNZ_OK) {
    if (!zip_extract_current_entry(uf, target_dir)) {
      return false;
    }
    err = unzGoToNextFile(uf);
  }

  return err == UNZ_END_OF_LIST_OF_FILE;
}

} // namespace zip

} // namespace xl
