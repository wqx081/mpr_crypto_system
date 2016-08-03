#include "crypto/symmetric_key.h"

#include <memory>
#include <string>

#include <glog/logging.h>
#include <gtest/gtest.h>

using namespace crypto;

TEST(SymmetricKeyTest, GenerateRandomKeyForAES256) {
  std::unique_ptr<SymmetricKey> key(SymmetricKey::GenerateRandomKey(SymmetricKey::AES, 256));
  ASSERT_TRUE(key);
  std::string raw_key;
  EXPECT_TRUE(key->GetRawKey(&raw_key));
  EXPECT_EQ(32U, raw_key.size());

  std::string hex_en_string = key->ToHexString();
  LOG(INFO) << "AES_256_KEY: " << hex_en_string;
  EXPECT_EQ(key->key(),
            SymmetricKey::FromHexString(hex_en_string)->key());
}

TEST(SymmetricKeyTest, GenerateRandomKeyForAES128) {
  std::unique_ptr<SymmetricKey> key(SymmetricKey::GenerateRandomKey(SymmetricKey::AES, 128));
  ASSERT_TRUE(key);
  std::string raw_key;
  EXPECT_TRUE(key->GetRawKey(&raw_key));
  EXPECT_EQ(16U, raw_key.size());

  std::string hex_en_string = key->ToHexString();
  LOG(INFO) << "AES_256_KEY: " << hex_en_string;
  EXPECT_EQ(key->key(),
            SymmetricKey::FromHexString(hex_en_string)->key());
}

TEST(SymmetricKeyTest, ImportDerivedKey) {
  std::unique_ptr<SymmetricKey> key1(SymmetricKey::DeriveKeyFromPassword(
      SymmetricKey::HMAC_SHA1, "password", "somesalt", 1024, 160));
  ASSERT_TRUE(key1);
  std::string raw_key1;
  EXPECT_TRUE(key1->GetRawKey(&raw_key1));

  std::unique_ptr<SymmetricKey> key2(SymmetricKey::Import(SymmetricKey::HMAC_SHA1, raw_key1));
  ASSERT_TRUE(key2);

  std::string raw_key2;
  EXPECT_TRUE(key2->GetRawKey(&raw_key2));

  EXPECT_EQ(raw_key1, raw_key2);
}


