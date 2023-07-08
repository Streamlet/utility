#include "crypto.h"
#include <loki/ScopeGuard.h>
#include <memory>
#ifdef _WIN32
// clang-format off
#include <Windows.h>
#include <WinCrypt.h>
// clang-format on
#else
#include <openssl/crypto.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#endif

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

#ifdef _WIN32

#define MD5_DIGEST_LENGTH 16
#define SHA_DIGEST_LENGTH 20
#define SHA224_DIGEST_LENGTH 28
#define SHA256_DIGEST_LENGTH 32
#define SHA384_DIGEST_LENGTH 48
#define SHA512_DIGEST_LENGTH 64
#define DIGEST_BUFFER_SIZE SHA512_DIGEST_LENGTH

std::string Hash(const void *data, size_t data_length, ALG_ID algid, DWORD hash_length) {
  HCRYPTPROV hProv = NULL;
  if (!CryptAcquireContext(&hProv, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CRYPT_SILENT))
    return "";
  LOKI_ON_BLOCK_EXIT(::CryptReleaseContext, hProv, 0);

  HCRYPTHASH hHash = NULL;
  if (!::CryptCreateHash(hProv, algid, 0, 0, &hHash)) {
    auto err = ::GetLastError();

    return "";
  }
  LOKI_ON_BLOCK_EXIT(::CryptDestroyHash, hHash);

  if (!::CryptHashData(hHash, (const BYTE *)data, (DWORD)data_length, 0))
    return "";

  BYTE digest[DIGEST_BUFFER_SIZE];
  if (!::CryptGetHashParam(hHash, HP_HASHVAL, digest, &hash_length, 0))
    return "";

  return DigestToHexString(digest, hash_length);
}

std::string MD5(const void *data, size_t length) {
  return Hash(data, length, CALG_MD5, MD5_DIGEST_LENGTH);
}
std::string SHA1(const void *data, size_t length) {
  return Hash(data, length, CALG_SHA1, SHA_DIGEST_LENGTH);
}
std::string SHA256(const void *data, size_t length) {
  return Hash(data, length, CALG_SHA_256, SHA256_DIGEST_LENGTH);
}
std::string SHA384(const void *data, size_t length) {
  return Hash(data, length, CALG_SHA_384, SHA384_DIGEST_LENGTH);
}
std::string SHA512(const void *data, size_t length) {
  return Hash(data, length, CALG_SHA_512, SHA512_DIGEST_LENGTH);
}

std::string MD5File(const std::filesystem::path &file_path) {
  FILE *f = _wfopen(file_path.c_str(), L"rb");
  if (f == nullptr)
    return "";
  LOKI_ON_BLOCK_EXIT(fclose, f);

  HCRYPTPROV hProv = NULL;
  if (!CryptAcquireContext(&hProv, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CRYPT_SILENT))
    return "";
  LOKI_ON_BLOCK_EXIT(::CryptReleaseContext, hProv, 0);

  HCRYPTHASH hHash = NULL;
  if (!::CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
    return "";
  LOKI_ON_BLOCK_EXIT(::CryptDestroyHash, hHash);

  const size_t BUFFER_SIZE = 1024;
  unsigned char buffer[BUFFER_SIZE] = {};
  size_t bytes_read = 0;
  while (!feof(f)) {
    bytes_read = fread(buffer, 1, BUFFER_SIZE, f);
    if (!::CryptHashData(hHash, buffer, (DWORD)bytes_read, 0))
      return "";
  }
  BYTE digest[MD5_DIGEST_LENGTH];
  DWORD hash_length = MD5_DIGEST_LENGTH;
  if (!::CryptGetHashParam(hHash, HP_HASHVAL, digest, &hash_length, 0))
    return "";
  return DigestToHexString(digest, hash_length);
}

std::string SHA1File(const std::filesystem::path &file_path) {
  FILE *f = _wfopen(file_path.c_str(), L"rb");
  if (f == nullptr)
    return "";
  LOKI_ON_BLOCK_EXIT(fclose, f);

  HCRYPTPROV hProv = NULL;
  if (!CryptAcquireContext(&hProv, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CRYPT_SILENT))
    return "";
  LOKI_ON_BLOCK_EXIT(::CryptReleaseContext, hProv, 0);

  HCRYPTHASH hHash = NULL;
  if (!::CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash))
    return "";
  LOKI_ON_BLOCK_EXIT(::CryptDestroyHash, hHash);

  const size_t BUFFER_SIZE = 1024;
  unsigned char buffer[BUFFER_SIZE] = {};
  size_t bytes_read = 0;
  while (!feof(f)) {
    bytes_read = fread(buffer, 1, BUFFER_SIZE, f);
    if (!::CryptHashData(hHash, buffer, (DWORD)bytes_read, 0))
      return "";
  }
  BYTE digest[SHA_DIGEST_LENGTH];
  DWORD hash_length = SHA_DIGEST_LENGTH;
  if (!::CryptGetHashParam(hHash, HP_HASHVAL, digest, &hash_length, 0))
    return "";
  return DigestToHexString(digest, hash_length);
}

std::string SHA256File(const std::filesystem::path &file_path) {
  FILE *f = _wfopen(file_path.c_str(), L"rb");
  if (f == nullptr)
    return "";
  LOKI_ON_BLOCK_EXIT(fclose, f);

  HCRYPTPROV hProv = NULL;
  if (!CryptAcquireContext(&hProv, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CRYPT_SILENT))
    return "";
  LOKI_ON_BLOCK_EXIT(::CryptReleaseContext, hProv, 0);

  HCRYPTHASH hHash = NULL;
  if (!::CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash))
    return "";
  LOKI_ON_BLOCK_EXIT(::CryptDestroyHash, hHash);

  const size_t BUFFER_SIZE = 1024;
  unsigned char buffer[BUFFER_SIZE] = {};
  size_t bytes_read = 0;
  while (!feof(f)) {
    bytes_read = fread(buffer, 1, BUFFER_SIZE, f);
    if (!::CryptHashData(hHash, buffer, (DWORD)bytes_read, 0))
      return "";
  }
  BYTE digest[SHA256_DIGEST_LENGTH];
  DWORD hash_length = SHA256_DIGEST_LENGTH;
  if (!::CryptGetHashParam(hHash, HP_HASHVAL, digest, &hash_length, 0))
    return "";
  return DigestToHexString(digest, hash_length);
}
std::string SHA384File(const std::filesystem::path &file_path) {
  FILE *f = _wfopen(file_path.c_str(), L"rb");
  if (f == nullptr)
    return "";
  LOKI_ON_BLOCK_EXIT(fclose, f);

  HCRYPTPROV hProv = NULL;
  if (!CryptAcquireContext(&hProv, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CRYPT_SILENT))
    return "";
  LOKI_ON_BLOCK_EXIT(::CryptReleaseContext, hProv, 0);

  HCRYPTHASH hHash = NULL;
  if (!::CryptCreateHash(hProv, CALG_SHA_384, 0, 0, &hHash))
    return "";
  LOKI_ON_BLOCK_EXIT(::CryptDestroyHash, hHash);

  const size_t BUFFER_SIZE = 1024;
  unsigned char buffer[BUFFER_SIZE] = {};
  size_t bytes_read = 0;
  while (!feof(f)) {
    bytes_read = fread(buffer, 1, BUFFER_SIZE, f);
    if (!::CryptHashData(hHash, buffer, (DWORD)bytes_read, 0))
      return "";
  }
  BYTE digest[SHA384_DIGEST_LENGTH];
  DWORD hash_length = SHA384_DIGEST_LENGTH;
  if (!::CryptGetHashParam(hHash, HP_HASHVAL, digest, &hash_length, 0))
    return "";
  return DigestToHexString(digest, hash_length);
}

std::string SHA512File(const std::filesystem::path &file_path) {
  FILE *f = _wfopen(file_path.c_str(), L"rb");
  if (f == nullptr)
    return "";
  LOKI_ON_BLOCK_EXIT(fclose, f);

  HCRYPTPROV hProv = NULL;
  if (!CryptAcquireContext(&hProv, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CRYPT_SILENT))
    return "";
  LOKI_ON_BLOCK_EXIT(::CryptReleaseContext, hProv, 0);

  HCRYPTHASH hHash = NULL;
  if (!::CryptCreateHash(hProv, CALG_SHA_512, 0, 0, &hHash))
    return "";
  LOKI_ON_BLOCK_EXIT(::CryptDestroyHash, hHash);

  const size_t BUFFER_SIZE = 1024;
  unsigned char buffer[BUFFER_SIZE] = {};
  size_t bytes_read = 0;
  while (!feof(f)) {
    bytes_read = fread(buffer, 1, BUFFER_SIZE, f);
    if (!::CryptHashData(hHash, buffer, (DWORD)bytes_read, 0))
      return "";
  }
  BYTE digest[SHA512_DIGEST_LENGTH];
  DWORD hash_length = SHA512_DIGEST_LENGTH;
  if (!::CryptGetHashParam(hHash, HP_HASHVAL, digest, &hash_length, 0))
    return "";
  return DigestToHexString(digest, hash_length);
}

#else

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

std::string MD5File(const std::filesystem::path &file_path) {
#ifdef _MSC_VER
  FILE *f = _wfopen(file_path.c_str(), L"rb");
#else
  FILE *f = fopen(file_path.c_str(), "rb");
#endif
  if (f == nullptr)
    return "";
  LOKI_ON_BLOCK_EXIT(fclose, f);
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
  ::OPENSSL_cleanse(&c, sizeof(c));
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
  LOKI_ON_BLOCK_EXIT(fclose, f);
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
  ::OPENSSL_cleanse(&c, sizeof(c));
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
  LOKI_ON_BLOCK_EXIT(fclose, f);
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
  ::OPENSSL_cleanse(&c, sizeof(c));
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
  LOKI_ON_BLOCK_EXIT(fclose, f);
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
  ::OPENSSL_cleanse(&c, sizeof(c));
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
  LOKI_ON_BLOCK_EXIT(fclose, f);
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
  ::OPENSSL_cleanse(&c, sizeof(c));
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
  LOKI_ON_BLOCK_EXIT(fclose, f);
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
  ::OPENSSL_cleanse(&c, sizeof(c));
  return DigestToHexString(m, SHA512_DIGEST_LENGTH);
}

#endif

std::string MD5(const std::string &s) {
  return MD5(s.data(), s.length());
}
std::string SHA1(const std::string &s) {
  return SHA1(s.data(), s.length());
}
#ifndef _WIN32
std::string SHA224(const std::string &s) {
  return SHA224(s.data(), s.length());
}
#endif
std::string SHA256(const std::string &s) {
  return SHA256(s.data(), s.length());
}
std::string SHA384(const std::string &s) {
  return SHA384(s.data(), s.length());
}
std::string SHA512(const std::string &s) {
  return SHA512(s.data(), s.length());
}

} // namespace crypto