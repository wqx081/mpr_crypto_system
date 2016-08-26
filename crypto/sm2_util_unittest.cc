#include "crypto/sm2_util.h"
#include "base/status.h"
#include "third_party/libsm2/MPRAsymmCrypt.h"

#include <thread>
#include <vector>

#include <gtest/gtest.h>

namespace crypto {

const uint32_t kKey256Bits = 256;
const uint32_t kKey192Bits = 192;


TEST(Sm2UtilTest, GenerateKeyPair) {
  uint8_t pub_key[1024] = {0};  
  int pub_key_len = 1024;
  uint8_t prv_key[1024] = {0};  
  int prv_key_len = 128;
  EXPECT_TRUE(Sm2Util::GenerateKeyPair(kKey256Bits,
                                       pub_key, &pub_key_len,
                                       prv_key, &prv_key_len).ok());    
  EXPECT_EQ(pub_key_len, 64);
  EXPECT_EQ(prv_key_len, 32);
  
  pub_key_len = prv_key_len = 1024; // Must be reset
  EXPECT_TRUE(Sm2Util::GenerateKeyPair(kKey192Bits,
                                       pub_key, &pub_key_len,
                                       prv_key, &prv_key_len).ok());    
  EXPECT_EQ(pub_key_len, 48);
  EXPECT_EQ(prv_key_len, 24);
}

TEST(Sm2UtilTest, EncryptDecrypt) {
  uint8_t pub_key[128] = {0};  
  int pub_key_len = 128;
  uint8_t prv_key[128] = {0};  
  int prv_key_len = 128;

  EXPECT_TRUE(Sm2Util::GenerateKeyPair(kKey256Bits,
                                       pub_key, &pub_key_len,
                                       prv_key, &prv_key_len).ok());    
  uint8_t text[] = "123456789abcdef0123456789abcdef";
  uint8_t cipher[256] = {0};
  int  cipher_len = 256;
  uint8_t decipher[256] = {0};
  int  decipher_len = 256;

  EXPECT_TRUE(Sm2Util::PublicEncrypt(pub_key, pub_key_len,
			             text, sizeof(text),
				     cipher, &cipher_len).ok());  

  //EXPECT_EQ(cipher_len, 256);
  EXPECT_TRUE(Sm2Util::PrivateDecrypt(prv_key, prv_key_len,
			              cipher, cipher_len,
				      decipher, &decipher_len).ok());
  LOG(INFO) << decipher_len;
}

TEST(Sm2UtilTest, SignVerify) {
  uint8_t pub_key[128] = {0};  
  int pub_key_len = 128;
  uint8_t prv_key[128] = {0};  
  int prv_key_len = 128;

  EXPECT_TRUE(Sm2Util::GenerateKeyPair(kKey256Bits,
                                       pub_key, &pub_key_len,
                                       prv_key, &prv_key_len).ok());    

  const uint8_t sign_id[] = "mpr_sign_id_dead";
  const int sign_id_len = sizeof(sign_id);
  uint8_t text[] = "Hello, CPP Guy";
  uint8_t signature[64] = {0};
  int signature_len = 64;

  EXPECT_TRUE(Sm2Util::Sign(prv_key, prv_key_len,
			    text, sizeof(text),
			    sign_id, sign_id_len,
			    signature, &signature_len).ok());

  EXPECT_TRUE(Sm2Util::Verify(pub_key, pub_key_len,
			      text, sizeof(text),
			      sign_id, sign_id_len,
			      signature, signature_len).ok());

  EXPECT_FALSE(Sm2Util::Verify(pub_key, pub_key_len,
         (const uint8_t *)"Hello, Java Pig", sizeof("Hello, Java Pig"),
			      sign_id, sign_id_len,
			      signature, signature_len).ok());
}

} // namespace crypto
