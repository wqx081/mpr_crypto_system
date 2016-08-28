#include "crypto/symmetric_key.h"
#include "crypto/symmetric_encryptor.h"
#include "crypto/symmetric_crypt.h"

#include "base/string_encode.h"

#ifdef MPR_SM4_TEST
#include "third_party/libsm4/MPRSymmCrypt.h"
#endif // MPR_SM4_TEST

#include "base/file.h"
#include "base/file_path.h"
#include "base/file_util.h"

#include <gtest/gtest.h>

TEST(SymmetricEncryptor, CBC_ts) {
  std::string key_string;
  base::ReadFileToString(base::FilePath("/home/wqx/Downloads/hls/video.key"),
		  &key_string);
  std::unique_ptr<crypto::SymmetricKey> key(
		  crypto::SymmetricKey::Import(
			  crypto::SymmetricKey::AES,
			  key_string));
  EXPECT_TRUE(key.get());

  std::string iv("the iv: 16 bytes");
  EXPECT_EQ(16U, iv.size());

  
  std::shared_ptr<crypto::SymmetricCrypt> crypt = 
    std::make_shared<crypto::CBCSymmetricCrypt>(iv);
  crypto::SymmetricEncryptor encryptor(key.get());
  encryptor.SetCrypt(crypt.get());

  std::string file_content;
  base::ReadFileToString(base::FilePath("/home/wqx/Downloads/hls/orig/my0.ts"),
		  &file_content);
  std::string file_cipher;
  EXPECT_TRUE(encryptor.Encrypt(file_content, &file_cipher));
  base::WriteFile(base::FilePath("/home/wqx/Downloads/hls/my0.ts"),
		  file_cipher.data(), file_cipher.size());
}

TEST(SymmetricEncryptor, CBC_EncryptAndDecrypt) {

  std::unique_ptr<crypto::SymmetricKey> key(
      crypto::SymmetricKey::DeriveKeyFromPassword(
      crypto::SymmetricKey::AES, "password", "saltiest", 1000, 256));
          
  EXPECT_TRUE(key.get());
  
  std::string iv("the iv: 16 bytes");
  EXPECT_EQ(16U, iv.size());

  std::shared_ptr<crypto::SymmetricCrypt> crypt = 
    std::make_shared<crypto::CBCSymmetricCrypt>(iv);

  crypto::SymmetricEncryptor encryptor(key.get());
  encryptor.SetCrypt(crypt.get());

  
  std::string plaintext("this is the plaintext");
  std::string ciphertext;
  EXPECT_TRUE(encryptor.Encrypt(plaintext, &ciphertext));
  
  EXPECT_LT(0U, ciphertext.size());
  
  std::string decrypted;
  EXPECT_TRUE(encryptor.Decrypt(ciphertext, &decrypted));
  
  EXPECT_EQ(plaintext, decrypted);
} 


TEST(SymmetricEncryptor, ECB_EncryptAndDecrypt) {
  std::unique_ptr<crypto::SymmetricKey> key = crypto::SymmetricKey::GenerateRandomKey(
    crypto::SymmetricKey::AES,
    128);
  EXPECT_TRUE(key.get());

  std::unique_ptr<crypto::SymmetricCrypt> crypt = make_unique<crypto::ECBSymmetricCrypt>();
  crypto::SymmetricEncryptor encryptor(key.get(), crypt.get());    

  std::string plaintext("Hello, World");
  std::string ciphertext;
  EXPECT_TRUE(encryptor.Encrypt(plaintext, &ciphertext));
  LOG(INFO) << "ciphertext: " << base::HexEncode(ciphertext);

  std::string plaintext1("hello, worldaabb");
  EXPECT_TRUE(encryptor.Encrypt(plaintext1, &ciphertext));
  LOG(INFO) << "ciphertext1: " << base::HexEncode(ciphertext);

  std::string decrypted;
  EXPECT_TRUE(encryptor.Decrypt(ciphertext, &decrypted));
  LOG(INFO) << "decrypted: " << decrypted;

#ifdef MPR_SM4_TEST
  // check for MPR aes
  int ret;
  uint8_t buf[1024] = {0};
  uint32_t buf_len = 1024;

  ret = MPRSymmCrypt_Init((unsigned char *)(key.get()->key().data()),
                          16, 0, 0, AES_ECB);
  CHECK(ret == 0);
  ret = MPRSymmCrypt_Decrypt((unsigned char *)(ciphertext.data()),
                             ciphertext.length(),
                             0,
                             buf,
                             &buf_len);
  CHECK(ret == 0);
  LOG(INFO) << "ciphertext length: " << ciphertext.length();
  LOG(INFO) << "last decrypt: " << (int)(buf[buf_len-1]);
  DCHECK((int)(buf[buf_len - 1]) == 4);
#endif // MPR_SM4_TEST
}
