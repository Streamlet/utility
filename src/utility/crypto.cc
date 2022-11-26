#include "crypto.h"
#include <loki/ScopeGuard.h>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// from <openssl/md5.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * ! MD5_LONG has to be at least 32 bits wide.                     !
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */
#define MD5_LONG unsigned int

#define MD5_CBLOCK 64
#define MD5_LBLOCK (MD5_CBLOCK / 4)
#define MD5_DIGEST_LENGTH 16

typedef struct MD5state_st {
  MD5_LONG A, B, C, D;
  MD5_LONG Nl, Nh;
  MD5_LONG data[MD5_LBLOCK];
  unsigned int num;
} MD5_CTX;

int MD5_Init(MD5_CTX *c);
int MD5_Update(MD5_CTX *c, const void *data, size_t len);
int MD5_Final(unsigned char *md, MD5_CTX *c);
unsigned char *MD5(const unsigned char *d, size_t n, unsigned char *md);
void MD5_Transform(MD5_CTX *c, const unsigned char *b);
#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////
// from <openssl/sha.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * ! SHA_LONG has to be at least 32 bits wide.                    !
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */
#define SHA_LONG unsigned int

#define SHA_LBLOCK 16
#define SHA_CBLOCK                                                                                                     \
  (SHA_LBLOCK * 4) /* SHA treats input data as a                                                                       \
                    * contiguous array of 32 bit wide                                                                  \
                    * big-endian values. */
#define SHA_LAST_BLOCK (SHA_CBLOCK - 8)
#define SHA_DIGEST_LENGTH 20

typedef struct SHAstate_st {
  SHA_LONG h0, h1, h2, h3, h4;
  SHA_LONG Nl, Nh;
  SHA_LONG data[SHA_LBLOCK];
  unsigned int num;
} SHA_CTX;

int SHA1_Init(SHA_CTX *c);
int SHA1_Update(SHA_CTX *c, const void *data, size_t len);
int SHA1_Final(unsigned char *md, SHA_CTX *c);
unsigned char *SHA1(const unsigned char *d, size_t n, unsigned char *md);
void SHA1_Transform(SHA_CTX *c, const unsigned char *data);

#define SHA256_CBLOCK                                                                                                  \
  (SHA_LBLOCK * 4) /* SHA-256 treats input data as a                                                                   \
                    * contiguous array of 32 bit wide                                                                  \
                    * big-endian values. */

typedef struct SHA256state_st {
  SHA_LONG h[8];
  SHA_LONG Nl, Nh;
  SHA_LONG data[SHA_LBLOCK];
  unsigned int num, md_len;
} SHA256_CTX;

int SHA224_Init(SHA256_CTX *c);
int SHA224_Update(SHA256_CTX *c, const void *data, size_t len);
int SHA224_Final(unsigned char *md, SHA256_CTX *c);
unsigned char *SHA224(const unsigned char *d, size_t n, unsigned char *md);
int SHA256_Init(SHA256_CTX *c);
int SHA256_Update(SHA256_CTX *c, const void *data, size_t len);
int SHA256_Final(unsigned char *md, SHA256_CTX *c);
unsigned char *SHA256(const unsigned char *d, size_t n, unsigned char *md);
void SHA256_Transform(SHA256_CTX *c, const unsigned char *data);

#define SHA224_DIGEST_LENGTH 28
#define SHA256_DIGEST_LENGTH 32
#define SHA384_DIGEST_LENGTH 48
#define SHA512_DIGEST_LENGTH 64

/*
 * Unlike 32-bit digest algorithms, SHA-512 *relies* on SHA_LONG64
 * being exactly 64-bit wide. See Implementation Notes in sha512.c
 * for further details.
 */
/*
 * SHA-512 treats input data as a
 * contiguous array of 64 bit
 * wide big-endian values.
 */
#define SHA512_CBLOCK (SHA_LBLOCK * 8)
#if (defined(_WIN32) || defined(_WIN64)) && !defined(__MINGW32__)
#define SHA_LONG64 unsigned __int64
#define U64(C) C##UI64
#elif defined(__arch64__)
#define SHA_LONG64 unsigned long
#define U64(C) C##UL
#else
#define SHA_LONG64 unsigned long long
#define U64(C) C##ULL
#endif

typedef struct SHA512state_st {
  SHA_LONG64 h[8];
  SHA_LONG64 Nl, Nh;
  union {
    SHA_LONG64 d[SHA_LBLOCK];
    unsigned char p[SHA512_CBLOCK];
  } u;
  unsigned int num, md_len;
} SHA512_CTX;

int SHA384_Init(SHA512_CTX *c);
int SHA384_Update(SHA512_CTX *c, const void *data, size_t len);
int SHA384_Final(unsigned char *md, SHA512_CTX *c);
unsigned char *SHA384(const unsigned char *d, size_t n, unsigned char *md);
int SHA512_Init(SHA512_CTX *c);
int SHA512_Update(SHA512_CTX *c, const void *data, size_t len);
int SHA512_Final(unsigned char *md, SHA512_CTX *c);
unsigned char *SHA512(const unsigned char *d, size_t n, unsigned char *md);
void SHA512_Transform(SHA512_CTX *c, const unsigned char *data);

#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////
// from <openssl/crypto.h>

#ifdef __cplusplus
extern "C" {
#endif

void OPENSSL_cleanse(void *ptr, size_t len);

#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////

namespace {

std::string DigestToHexString(unsigned char *digest, size_t length) {
  std::string hex;
  hex.resize(length * 2);
  for (size_t i = 0; i < length; ++i)
    sprintf(hex.data() + i * 2, "%02x", digest[i]);
  return std::move(hex);
}

} // namespace

namespace crypto {

std::string MD5(const void *data, size_t length) {
  unsigned char *digest = ::MD5(static_cast<const unsigned char *>(data), length, nullptr);
  return DigestToHexString(digest, MD5_DIGEST_LENGTH);
}
std::string MD5(const char *file_path) {
  FILE *f = fopen(file_path, "rb");
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

std::string SHA1(const void *data, size_t length) {
  unsigned char *digest = ::SHA1(static_cast<const unsigned char *>(data), length, nullptr);
  return DigestToHexString(digest, SHA_DIGEST_LENGTH);
}
std::string SHA1(const char *file_path) {
  FILE *f = fopen(file_path, "rb");
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

std::string SHA224(const void *data, size_t length) {
  unsigned char *digest = ::SHA224(static_cast<const unsigned char *>(data), length, nullptr);
  return DigestToHexString(digest, SHA224_DIGEST_LENGTH);
}
std::string SHA224(const char *file_path) {
  FILE *f = fopen(file_path, "rb");
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

std::string SHA256(const void *data, size_t length) {
  unsigned char *digest = ::SHA256(static_cast<const unsigned char *>(data), length, nullptr);
  return DigestToHexString(digest, SHA256_DIGEST_LENGTH);
}
std::string SHA256(const char *file_path) {
  FILE *f = fopen(file_path, "rb");
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

std::string SHA384(const void *data, size_t length) {
  unsigned char *digest = ::SHA384(static_cast<const unsigned char *>(data), length, nullptr);
  return DigestToHexString(digest, SHA384_DIGEST_LENGTH);
}
std::string SHA384(const char *file_path) {
  FILE *f = fopen(file_path, "rb");
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

std::string SHA512(const void *data, size_t length) {
  unsigned char *digest = ::SHA512(static_cast<const unsigned char *>(data), length, nullptr);
  return DigestToHexString(digest, SHA512_DIGEST_LENGTH);
}
std::string SHA512(const char *file_path) {
  FILE *f = fopen(file_path, "rb");
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

} // namespace crypto