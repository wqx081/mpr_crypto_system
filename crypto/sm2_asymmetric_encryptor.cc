#include "crypto/sm2_asymmetric_encryptor.h"
#include "crypto/sm2_util.h"

#include "base/status.h"

#include <mutex>
#include <thread>

namespace crypto {

const int Sm2AsymmetricEncryptor::kMaxSm2EncryptBlockSize;
const int Sm2AsymmetricEncryptor::kMaxSm2EncryptBufferSize;
const int Sm2AsymmetricEncryptor::kMaxSm2DecryptBufferSize;
const int Sm2AsymmetricEncryptor::kMaxPublicKeySize;
const int Sm2AsymmetricEncryptor::kMaxPrivateKeySize;

Sm2AsymmetricEncryptor::Sm2AsymmetricEncryptor() {}

// static
AsymmetricEncryptor* Sm2AsymmetricEncryptor::GetInstance() {
  static Sm2AsymmetricEncryptor instance;
  return &instance;
}

std::unique_ptr<AsymmetricKey> 
Sm2AsymmetricEncryptor::Create(KeyBits key_bits) {
  // Lock
  std::lock_guard<std::mutex> lock(access_mutex_);

  if (!AcceptKeyBits(key_bits)) {
    //LOG
    return nullptr;
  }

  Byte public_key[kMaxPublicKeySize];
  Byte private_key[kMaxPrivateKeySize];
  int public_key_len = kMaxPublicKeySize+1;
  int private_key_len = kMaxPrivateKeySize+1;

  base::Status status = Sm2Util::GenerateKeyPair(key_bits,
		                       public_key, &public_key_len,
				       private_key, &private_key_len);
  if (!status.ok()) {
    //LOG
    return nullptr;
  }

  return Sm2Key::FromBuffer(public_key, public_key_len,
		            private_key, private_key_len);
}

bool Sm2AsymmetricEncryptor::PublicKeyEncrypt(
		std::unique_ptr<AsymmetricKey>& key,
		InputStream* plaintext,
		OutputStream* output) {
  // LOCK
  std::lock_guard<std::mutex> lock(access_mutex_);
  
  ByteVector public_key;
  if (!key->ExportPublicKey(&public_key)) {
    return false; 
  }

  Byte cipher_buffer[kMaxSm2EncryptBufferSize];
  int cipher_len = kMaxSm2EncryptBufferSize;
  base::Status status;

  while (plaintext->Available()) {
    ByteVector block;
    block.resize(kMaxSm2EncryptBlockSize); 
    plaintext->Read(&block);

    cipher_len = kMaxSm2EncryptBufferSize;
    status = Sm2Util::PublicEncrypt(public_key.data(), public_key.size(),
		                    block.data(), block.size(),
				    cipher_buffer, &cipher_len);
    if (!status.ok()) {
      return false;
    }
    output->Write(cipher_buffer, 0, cipher_len);
  }
  return true;
}

bool Sm2AsymmetricEncryptor::PrivateKeyDecrypt(
		std::unique_ptr<AsymmetricKey>& key,
		InputStream* cipher,
		OutputStream* output) {
  // LOCK
  std::lock_guard<std::mutex> lock(access_mutex_);

  ByteVector private_key;
  if (!key->ExportPrivateKey(&private_key)) {
    return false; 
  }
  
  Byte plaintext_buffer[kMaxSm2DecryptBlockSize];
  int plaintext_len = kMaxSm2DecryptBufferSize;
  base::Status status;

  while (cipher->Available()) {
    ByteVector block;
    block.resize(kMaxSm2DecryptBufferSize); 
    size_t n = cipher->Read(&block);

    plaintext_len = kMaxSm2DecryptBufferSize;
    status = Sm2Util::PrivateDecrypt(private_key.data(), private_key.size(),
		                     block.data(), n,
				     plaintext_buffer, &plaintext_len);
    if (!status.ok()) {
      return false;
    }
    output->Write(plaintext_buffer, 0, plaintext_len);
  
  }
  return true;  
}

} // namespace crypto
