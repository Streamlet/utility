#include <gtest/gtest.h>
#include <xl/crypto>

TEST(crypto_test, normal) {
  ASSERT_EQ(xl::crypto::md5(""), "d41d8cd98f00b204e9800998ecf8427e");
  ASSERT_EQ(xl::crypto::sha1(""), "da39a3ee5e6b4b0d3255bfef95601890afd80709");
  ASSERT_EQ(xl::crypto::sha224(""), "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f");
  ASSERT_EQ(xl::crypto::sha256(""), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
  ASSERT_EQ(xl::crypto::sha384(""),
            "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b");
  ASSERT_EQ(xl::crypto::sha512(""),
            "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318"
            "d2877eec2f63b931bd47417a81a538327af927da3e");

  ASSERT_EQ(xl::crypto::md5("abc123"), "e99a18c428cb38d5f260853678922e03");
  ASSERT_EQ(xl::crypto::sha1("abc123"), "6367c48dd193d56ea7b0baad25b19455e529f5ee");
  ASSERT_EQ(xl::crypto::sha224("abc123"), "5c69bb695cc29b93d655e1a4bb5656cda624080d686f74477ea09349");
  ASSERT_EQ(xl::crypto::sha256("abc123"), "6ca13d52ca70c883e0f0bb101e425a89e8624de51db2d2392593af6a84118090");
  ASSERT_EQ(xl::crypto::sha384("abc123"),
            "a31d79891919cad24f3264479d76884f581bee32e86778373db3a124de975dd86a40fc7f399b331133b281ab4b11a6ca");
  ASSERT_EQ(xl::crypto::sha512("abc123"),
            "c70b5dd9ebfb6f51d09d4132b7170c9d20750a7852f00680f65658f0310e810056e6763c34c9a00b"
            "0e940076f54495c169fc2302cceb312039271c43469507dc");
}
