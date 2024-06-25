#include "native_string.h"
#include <string>

namespace xl {

namespace crypto {

std::string md5(const void *data, size_t length);
std::string md5(const std::string &s);
std::string md5(const TCHAR *file_path);
std::string sha1(const void *data, size_t length);
std::string sha1(const std::string &s);
std::string sha1(const TCHAR *file_path);
std::string sha224(const void *data, size_t length);
std::string sha224(const std::string &s);
std::string sha224(const TCHAR *file_path);
std::string sha256(const void *data, size_t length);
std::string sha256(const std::string &s);
std::string sha256(const TCHAR *file_path);
std::string sha384(const void *data, size_t length);
std::string sha384(const std::string &s);
std::string sha384(const TCHAR *file_path);
std::string sha512(const void *data, size_t length);
std::string sha512(const std::string &s);
std::string sha512(const TCHAR *file_path);

} // namespace crypto

} // namespace xl
