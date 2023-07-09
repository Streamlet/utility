#include "crypto.h"
#include <boost/scope_exit.hpp>
#include <memory>
#include <openssl/crypto.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

namespace {

std::string DigestToHexString(unsigned char *digest, size_t length) {
  std::string hex;
  hex.resize(length * 2);
  for (size_t i = 0; i < length; ++i)
    snprintf(hex.data() + i * 2, 3, "%02x", digest[i]);
  return std::move(hex);
}

} // namespace

namespace crypto {

std::string MD5(const void *data, size_t length) {
  unsigned char *digest = ::MD5(static_cast<const unsigned char *>(data), length, nullptr);
  return DigestToHexString(digest, MD5_DIGEST_LENGTH);
}
std::string SHA1(const void *data, size_t length) {
  unsigned char *digest = ::SHA1(static_cast<const unsigned char *>(data), length, nullptr);
  return DigestToHexString(digest, SHA_DIGEST_LENGTH);
}
std::string SHA224(const void *data, size_t length) {
  unsigned char *digest = ::SHA224(static_cast<const unsigned char *>(data), length, nullptr);
  return DigestToHexString(digest, SHA224_DIGEST_LENGTH);
}
std::string SHA256(const void *data, size_t length) {
  unsigned char *digest = ::SHA256(static_cast<const unsigned char *>(data), length, nullptr);
  return DigestToHexString(digest, SHA256_DIGEST_LENGTH);
}
std::string SHA384(const void *data, size_t length) {
  unsigned char *digest = ::SHA384(static_cast<const unsigned char *>(data), length, nullptr);
  return DigestToHexString(digest, SHA384_DIGEST_LENGTH);
}
std::string SHA512(const void *data, size_t length) {
  unsigned char *digest = ::SHA512(static_cast<const unsigned char *>(data), length, nullptr);
  return DigestToHexString(digest, SHA512_DIGEST_LENGTH);
}

std::string MD5(const std::string &s) {
  return MD5(s.data(), s.length());
}
std::string SHA1(const std::string &s) {
  return SHA1(s.data(), s.length());
}
std::string SHA224(const std::string &s) {
  return SHA224(s.data(), s.length());
}
std::string SHA256(const std::string &s) {
  return SHA256(s.data(), s.length());
}
std::string SHA384(const std::string &s) {
  return SHA384(s.data(), s.length());
}
std::string SHA512(const std::string &s) {
  return SHA512(s.data(), s.length());
}

std::string MD5File(const std::filesystem::path &file_path) {
#ifdef _MSC_VER
  FILE *f = _wfopen(file_path.c_str(), L"rb");
#else
  FILE *f = fopen(file_path.c_str(), "rb");
#endif
  if (f == nullptr)
    return "";
  BOOST_SCOPE_EXIT(f) {
    fclose(f);
  }
  BOOST_SCOPE_EXIT_END
  MD5_CTX c;
  static unsigned char m[MD5_DIGEST_LENGTH];
  ::MD5_Init(&c);
  const size_t BUFFER_SIZE = 1024;
  unsigned char buffer[BUFFER_SIZE] = {};
  size_t bytes_read = 0;
  while (!feof(f)) {
    bytes_read = fread(buffer, 1, BUFFER_SIZE, f);
    ::MD5_Update(&c, buffer, bytes_read);
  }
  ::MD5_Final(m, &c);
  return DigestToHexString(m, MD5_DIGEST_LENGTH);
}

std::string SHA1File(const std::filesystem::path &file_path) {
#ifdef _MSC_VER
  FILE *f = _wfopen(file_path.c_str(), L"rb");
#else
  FILE *f = fopen(file_path.c_str(), "rb");
#endif
  if (f == nullptr)
    return "";
  BOOST_SCOPE_EXIT(f) {
    fclose(f);
  }
  BOOST_SCOPE_EXIT_END
  SHA_CTX c;
  static unsigned char m[SHA_DIGEST_LENGTH];
  ::SHA1_Init(&c);
  const size_t BUFFER_SIZE = 1024;
  unsigned char buffer[BUFFER_SIZE] = {};
  size_t bytes_read = 0;
  while (!feof(f)) {
    bytes_read = fread(buffer, 1, BUFFER_SIZE, f);
    ::SHA1_Update(&c, buffer, bytes_read);
  }
  ::SHA1_Final(m, &c);
  return DigestToHexString(m, SHA_DIGEST_LENGTH);
}

std::string SHA224File(const std::filesystem::path &file_path) {
#ifdef _MSC_VER
  FILE *f = _wfopen(file_path.c_str(), L"rb");
#else
  FILE *f = fopen(file_path.c_str(), "rb");
#endif
  if (f == nullptr)
    return "";
  BOOST_SCOPE_EXIT(f) {
    fclose(f);
  }
  BOOST_SCOPE_EXIT_END
  SHA256_CTX c;
  static unsigned char m[SHA224_DIGEST_LENGTH];
  ::SHA224_Init(&c);
  const size_t BUFFER_SIZE = 1024;
  unsigned char buffer[BUFFER_SIZE] = {};
  size_t bytes_read = 0;
  while (!feof(f)) {
    bytes_read = fread(buffer, 1, BUFFER_SIZE, f);
    ::SHA256_Update(&c, buffer, bytes_read);
  }
  ::SHA256_Final(m, &c);
  return DigestToHexString(m, SHA224_DIGEST_LENGTH);
}

std::string SHA256File(const std::filesystem::path &file_path) {
#ifdef _MSC_VER
  FILE *f = _wfopen(file_path.c_str(), L"rb");
#else
  FILE *f = fopen(file_path.c_str(), "rb");
#endif
  if (f == nullptr)
    return "";
  BOOST_SCOPE_EXIT(f) {
    fclose(f);
  }
  BOOST_SCOPE_EXIT_END
  SHA256_CTX c;
  static unsigned char m[SHA256_DIGEST_LENGTH];
  ::SHA256_Init(&c);
  const size_t BUFFER_SIZE = 1024;
  unsigned char buffer[BUFFER_SIZE] = {};
  size_t bytes_read = 0;
  while (!feof(f)) {
    bytes_read = fread(buffer, 1, BUFFER_SIZE, f);
    ::SHA256_Update(&c, buffer, bytes_read);
  }
  ::SHA256_Final(m, &c);
  return DigestToHexString(m, SHA256_DIGEST_LENGTH);
}

std::string SHA384File(const std::filesystem::path &file_path) {
#ifdef _MSC_VER
  FILE *f = _wfopen(file_path.c_str(), L"rb");
#else
  FILE *f = fopen(file_path.c_str(), "rb");
#endif
  if (f == nullptr)
    return "";
  BOOST_SCOPE_EXIT(f) {
    fclose(f);
  }
  BOOST_SCOPE_EXIT_END
  SHA512_CTX c;
  static unsigned char m[SHA384_DIGEST_LENGTH];
  ::SHA384_Init(&c);
  const size_t BUFFER_SIZE = 1024;
  unsigned char buffer[BUFFER_SIZE] = {};
  size_t bytes_read = 0;
  while (!feof(f)) {
    bytes_read = fread(buffer, 1, BUFFER_SIZE, f);
    ::SHA512_Update(&c, buffer, bytes_read);
  }
  ::SHA512_Final(m, &c);
  return DigestToHexString(m, SHA384_DIGEST_LENGTH);
}

std::string SHA512File(const std::filesystem::path &file_path) {
#ifdef _MSC_VER
  FILE *f = _wfopen(file_path.c_str(), L"rb");
#else
  FILE *f = fopen(file_path.c_str(), "rb");
#endif
  if (f == nullptr)
    return "";
  BOOST_SCOPE_EXIT(f) {
    fclose(f);
  }
  BOOST_SCOPE_EXIT_END
  SHA512_CTX c;
  static unsigned char m[SHA512_DIGEST_LENGTH];
  ::SHA512_Init(&c);
  const size_t BUFFER_SIZE = 1024;
  unsigned char buffer[BUFFER_SIZE] = {};
  size_t bytes_read = 0;
  while (!feof(f)) {
    bytes_read = fread(buffer, 1, BUFFER_SIZE, f);
    ::SHA512_Update(&c, buffer, bytes_read);
  }
  ::SHA512_Final(m, &c);
  return DigestToHexString(m, SHA512_DIGEST_LENGTH);
}

} // namespace crypto