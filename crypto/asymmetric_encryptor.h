//@Author: WangQixiang (wangqx at mpreader.com)
//@Date: 2016/08/27
//
#ifndef CRYPTO_ASYMMETRIC_ENCRYPTOR_H_
#define CRYPTO_ASYMMETRIC_ENCRYPTOR_H_

#include <memory>
#include "crypto/asymmetric_key.h"
#include "crypto/input_stream.h"
#include "crypto/output_stream.h"

namespace crypto {

class AsymmetricEncryptor {
 public:
  enum KeyBits {
    kKeyBitsNone = 0,
    kKey192Bits = 192, // sm2 supported
    kKey256Bits = 256, // sm2 supported
    kKey1024Bits = 1024, // RSA supported
  };

  virtual ~AsymmetricEncryptor() {}
    
  // APIs
  virtual bool AcceptKeyBits(KeyBits key_bits) = 0;

  virtual std::unique_ptr<AsymmetricKey> Create(KeyBits key_bits) = 0;

  virtual bool PublicKeyEncrypt(std::unique_ptr<AsymmetricKey>& key,
                                InputStream* plaintext,
                                OutputStream* output) = 0;
  virtual bool PrivateKeyDecrypt(std::unique_ptr<AsymmetricKey>& key,
                                 InputStream*  cipher,
                                 OutputStream* output) = 0;

//TODO(wqx):
#if 0
  virtual bool Sign(std::unique_ptr<AsymmetricKey>& key,
                    InputStream* data,
                    const Byte* sign_id, int sign_id_len, // TODO
                    OutputStream* signature) = 0;

  virtual bool Verify(std::unique_ptr<AsymmetricKey>& key,
                      InputStream* data,
                      InputStream* signature,
                      const Byte* sign_id, int sign_id_len) = 0; // TODO
#endif

};

} // namespace crypto
#endif // CRYPTO_ASYMMETRIC_ENCRYPTO_H_
