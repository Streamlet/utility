#include <string>

namespace crypto {

std::string MD5(const void *data, size_t length);
std::string MD5(const char *file_path);
std::string SHA1(const void *data, size_t length);
std::string SHA1(const char *file_path);
std::string SHA224(const void *data, size_t length);
std::string SHA224(const char *file_path);
std::string SHA256(const void *data, size_t length);
std::string SHA256(const char *file_path);
std::string SHA384(const void *data, size_t length);
std::string SHA384(const char *file_path);
std::string SHA512(const void *data, size_t length);
std::string SHA512(const char *file_path);

} // namespace crypto
