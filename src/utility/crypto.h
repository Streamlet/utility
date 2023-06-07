#include <filesystem>
#include <string>

namespace crypto {

std::string MD5(const void *data, size_t length);
std::string MD5(const std::filesystem::path &file_path);
std::string SHA1(const void *data, size_t length);
std::string SHA1(const std::filesystem::path &file_path);
std::string SHA224(const void *data, size_t length);
std::string SHA224(const std::filesystem::path &file_path);
std::string SHA256(const void *data, size_t length);
std::string SHA256(const std::filesystem::path &file_path);
std::string SHA384(const void *data, size_t length);
std::string SHA384(const std::filesystem::path &file_path);
std::string SHA512(const void *data, size_t length);
std::string SHA512(const std::filesystem::path &file_path);

} // namespace crypto
