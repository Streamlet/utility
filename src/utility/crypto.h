#include <filesystem>
#include <string>

namespace crypto {

std::string MD5(const void *data, size_t length);
std::string MD5(const std::string &s);
std::string MD5File(const std::filesystem::path &file_path);
std::string SHA1(const void *data, size_t length);
std::string SHA1(const std::string &s);
std::string SHA1File(const std::filesystem::path &file_path);
std::string SHA224(const void *data, size_t length);
std::string SHA224(const std::string &s);
std::string SHA224File(const std::filesystem::path &file_path);
std::string SHA256(const void *data, size_t length);
std::string SHA256(const std::string &s);
std::string SHA256File(const std::filesystem::path &file_path);
std::string SHA384(const void *data, size_t length);
std::string SHA384(const std::string &s);
std::string SHA384File(const std::filesystem::path &file_path);
std::string SHA512(const void *data, size_t length);
std::string SHA512(const std::string &s);
std::string SHA512File(const std::filesystem::path &file_path);

} // namespace crypto
