//@Author: WangQixiang (wangqx at mpreader.com)
//@Date: 2016/08/27
//
#ifndef CRYPTO_SM2_ASYMMETRIC_ENCRYPTOR_H_
#define CRYPTO_SM2_ASYMMETRIC_ENCRYPTOR_H_
#include "base/macros.h"
#include "crypto/type.h"
#include "crypto/asymmetric_encryptor.h"

#include <utility>
#include <mutex>

namespace crypto {

class Sm2AsymmetricEncryptor : public AsymmetricEncryptor {
 public:
  virtual ~Sm2AsymmetricEncryptor() {}

  // Signleton
  //static Sm2AsymmetricEncryptor* GetInstance();
  static AsymmetricEncryptor* GetInstance();

  // APIs
  virtual bool AcceptKeyBits(KeyBits key_bits) override {
    switch (key_bits) {
      case kKey192Bits:
      case kKey256Bits:
        return true;	       
      default:
	return false;
    }
  }

  virtual std::unique_ptr<AsymmetricKey> Create(KeyBits key_bits) override;

  virtual bool PublicKeyEncrypt(std::unique_ptr<AsymmetricKey>& key,
		                InputStream* plaintext,
				OutputStream* output) override;

  virtual bool PrivateKeyDecrypt(std::unique_ptr<AsymmetricKey>& key,
		                 InputStream* cipher,
				 OutputStream* output) override;
#if 0
  virtual bool Sign(std::unique_ptr<AsymmetricKey>& key,
		    const InputStream* data,
		    const Byte* sign_id, int sign_id_len,
		    OutputStream* signature) override;
  virtual bool Verify(std::unique_ptr<AsymmetricKey>& key,
		      const InputStream* data,
		      const InputStream* signature,
		      const Byte* sign_id, int sign_id_len) override;
#endif
 private:
  static const int kMaxSm2EncryptBlockSize = 32;   // bytes
  static const int kMaxSm2DecryptBlockSize = 32;   // bytes
  static const int kMaxSm2EncryptBufferSize = 256; // bytes
  static const int kMaxSm2DecryptBufferSize = 256; // bytes
  static const int kMaxPublicKeySize = 64;         // bytes
  static const int kMaxPrivateKeySize = 32;        // bytes
  Sm2AsymmetricEncryptor();
  std::mutex access_mutex_;

  DISALLOW_COPY_AND_ASSIGN(Sm2AsymmetricEncryptor);
};

} // namespace crypto
#endif // CRYPTO_SM2_ASYMMETRIC_ENCRYPTOR_H_
