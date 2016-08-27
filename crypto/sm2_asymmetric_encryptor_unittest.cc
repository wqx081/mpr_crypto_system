#include "crypto/asymmetric_encryptor.h"
#include "crypto/sm2_asymmetric_encryptor.h"

#include "crypto/memory_input_stream.h"
#include "crypto/memory_output_stream.h"

#include <gtest/gtest.h>
#include <string>

namespace {

const char* kPlaintext0="Hello, CPP Guy!"; // len == 15 < 32
const char* kPlaintext1="Hello, CPP Guy!f1234567890abcdef"; // len == 32
const char* kPlaintext2="Hello, CPP Guy!f1234567890abcdef0";// len == 33

} // namespace

namespace crypto {

TEST(Sm2AsymmetricEncryptor, Create) {
  std::unique_ptr<AsymmetricKey> key = 
	  Sm2AsymmetricEncryptor::GetInstance()->Create(
			  AsymmetricEncryptor::kKey192Bits);

  ByteVector public_key;
  ByteVector private_key;
  EXPECT_TRUE(key);
  EXPECT_TRUE(key->ExportPublicKey(&public_key));
  EXPECT_TRUE(key->ExportPrivateKey(&private_key));
  EXPECT_EQ(48, public_key.size());
  EXPECT_EQ(24, private_key.size());

  public_key.clear();
  private_key.clear();
  std::unique_ptr<AsymmetricKey> key1 = 
	  Sm2AsymmetricEncryptor::GetInstance()->Create(
			  AsymmetricEncryptor::kKey256Bits);
  EXPECT_TRUE(key1);
  EXPECT_TRUE(key1->ExportPublicKey(&public_key));
  EXPECT_TRUE(key1->ExportPrivateKey(&private_key));
  EXPECT_EQ(64, public_key.size());
  EXPECT_EQ(32, private_key.size());

  // sm2 *NOT* supported 1024 bits
  std::unique_ptr<AsymmetricKey> key2 =
	  Sm2AsymmetricEncryptor::GetInstance()->Create(
			  AsymmetricEncryptor::kKey1024Bits);
  EXPECT_EQ(nullptr, key2);
}

bool Sm2EncryptDecrypt(AsymmetricEncryptor::KeyBits key_bits,
		       const char* plaint, size_t len) {
  InputStream* encrypt_is(new MemoryInputStream((const Byte*)plaint, len));
  OutputStream* encrypt_os(new MemoryOutputStream);			                      
  std::unique_ptr<AsymmetricKey> key =
	  Sm2AsymmetricEncryptor::GetInstance()->Create(key_bits);
  
  EXPECT_TRUE(key);
  EXPECT_TRUE(Sm2AsymmetricEncryptor::GetInstance()->PublicKeyEncrypt(
			  key,
			  encrypt_is, encrypt_os));

  std::string cipher_string = encrypt_os->ToString();

  InputStream* decrypt_is(new MemoryInputStream(cipher_string));
  OutputStream* decrypt_os(new MemoryOutputStream);

  EXPECT_TRUE(Sm2AsymmetricEncryptor::GetInstance()->PrivateKeyDecrypt(
			  key,
			  decrypt_is, decrypt_os));
  EXPECT_EQ(0, memcmp(plaint, decrypt_os->ToString().data(), len));

  delete encrypt_is;
  delete encrypt_os;
  delete decrypt_is;
  delete decrypt_os;
  return true;
}

TEST(Sm2AsymmetricEncryptor, EncryptDecrypt) {
  ////////////////// 192 bits
  EXPECT_TRUE(Sm2EncryptDecrypt(AsymmetricEncryptor::kKey192Bits,
			        kPlaintext0, 
				sizeof(kPlaintext0)));
  EXPECT_TRUE(Sm2EncryptDecrypt(AsymmetricEncryptor::kKey192Bits,
			        kPlaintext1, 
				sizeof(kPlaintext1)));
  EXPECT_TRUE(Sm2EncryptDecrypt(AsymmetricEncryptor::kKey192Bits,
			        kPlaintext2, 
				sizeof(kPlaintext2)));
  //////////////// 256 bits
  EXPECT_TRUE(Sm2EncryptDecrypt(AsymmetricEncryptor::kKey256Bits,
			        kPlaintext0, 
				sizeof(kPlaintext0)));
  EXPECT_TRUE(Sm2EncryptDecrypt(AsymmetricEncryptor::kKey256Bits,
			        kPlaintext1, 
				sizeof(kPlaintext1)));
  EXPECT_TRUE(Sm2EncryptDecrypt(AsymmetricEncryptor::kKey256Bits,
			        kPlaintext2, 
				sizeof(kPlaintext2)));
}

}  // namespace crypto
