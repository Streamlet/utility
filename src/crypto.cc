// MIT License
//
// Copyright (c) 2022 Streamlet (streamlet@outlook.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <memory>
#include <openssl/crypto.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <xl/crypto>
#include <xl/scope_exit>

namespace xl {

namespace {

std::string hex(unsigned char *digest, size_t length) {
  std::string hex;
  hex.resize(length * 2);
  for (size_t i = 0; i < length; ++i) {
    snprintf(&hex[0] + i * 2, 3, "%02x", digest[i]);
  }
  return hex;
}

} // namespace

namespace crypto {

std::string md5(const void *data, size_t length) {
  unsigned char *digest = ::MD5(static_cast<const unsigned char *>(data), length, nullptr);
  return hex(digest, MD5_DIGEST_LENGTH);
}
std::string sha1(const void *data, size_t length) {
  unsigned char *digest = ::SHA1(static_cast<const unsigned char *>(data), length, nullptr);
  return hex(digest, SHA_DIGEST_LENGTH);
}
std::string sha224(const void *data, size_t length) {
  unsigned char *digest = ::SHA224(static_cast<const unsigned char *>(data), length, nullptr);
  return hex(digest, SHA224_DIGEST_LENGTH);
}
std::string sha256(const void *data, size_t length) {
  unsigned char *digest = ::SHA256(static_cast<const unsigned char *>(data), length, nullptr);
  return hex(digest, SHA256_DIGEST_LENGTH);
}
std::string sha384(const void *data, size_t length) {
  unsigned char *digest = ::SHA384(static_cast<const unsigned char *>(data), length, nullptr);
  return hex(digest, SHA384_DIGEST_LENGTH);
}
std::string sha512(const void *data, size_t length) {
  unsigned char *digest = ::SHA512(static_cast<const unsigned char *>(data), length, nullptr);
  return hex(digest, SHA512_DIGEST_LENGTH);
}

std::string md5(const std::string &s) {
  return md5(s.data(), s.length());
}
std::string sha1(const std::string &s) {
  return sha1(s.data(), s.length());
}
std::string sha224(const std::string &s) {
  return sha224(s.data(), s.length());
}
std::string sha256(const std::string &s) {
  return sha256(s.data(), s.length());
}
std::string sha384(const std::string &s) {
  return sha384(s.data(), s.length());
}
std::string sha512(const std::string &s) {
  return sha512(s.data(), s.length());
}

std::string md5_file(const TCHAR *file_path) {
  FILE *f = _tfopen(file_path, _T("rb"));
  if (f == nullptr) {
    return "";
  }
  XL_ON_BLOCK_EXIT(fclose, f);
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
  return hex(m, MD5_DIGEST_LENGTH);
}

std::string sha1_file(const TCHAR *file_path) {
  FILE *f = _tfopen(file_path, _T("rb"));
  if (f == nullptr) {
    return "";
  }
  XL_ON_BLOCK_EXIT(fclose, f);
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
  return hex(m, SHA_DIGEST_LENGTH);
}

std::string sha224_file(const TCHAR *file_path) {
  FILE *f = _tfopen(file_path, _T("rb"));
  if (f == nullptr) {
    return "";
  }
  XL_ON_BLOCK_EXIT(fclose, f);
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
  return hex(m, SHA224_DIGEST_LENGTH);
}

std::string sha256_file(const TCHAR *file_path) {
  FILE *f = _tfopen(file_path, _T("rb"));
  if (f == nullptr) {
    return "";
  }
  XL_ON_BLOCK_EXIT(fclose, f);
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
  return hex(m, SHA256_DIGEST_LENGTH);
}

std::string sha384_file(const TCHAR *file_path) {
  FILE *f = _tfopen(file_path, _T("rb"));
  if (f == nullptr) {
    return "";
  }
  XL_ON_BLOCK_EXIT(fclose, f);
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
  return hex(m, SHA384_DIGEST_LENGTH);
}

std::string sha512_file(const TCHAR *file_path) {
  FILE *f = _tfopen(file_path, _T("rb"));
  if (f == nullptr) {
    return "";
  }
  XL_ON_BLOCK_EXIT(fclose, f);
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
  return hex(m, SHA512_DIGEST_LENGTH);
}

} // namespace crypto

} // namespace xl
