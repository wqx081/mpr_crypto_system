//@Author: WangQixiang (wangqx at mpreader.com)
//@Date: 2016/08/27
//
#ifndef CRYPTO_ASYMMETRIC_ENCRYPTO_H_
#define CRYPTO_ASYMMETRIC_ENCRYPTO_H_

#include <memory>
#include "crypto/asymmetric_key.h"
#include "crypto/input_stream.h"
#include "crypto/output_stream.h"

namespace crypto {

// Forwards
class AsymmetricKey;
class InputStream;
class OutputStream;

class AsymmetricEncryptor {
 public:
  enum KeyBits {
    kKeyBitsNone = 0,
    kKey192Bits, // sm2 supported
    kKey256Bits, // sm2 supported
    kKey1024Bits, // RSA supported
  };

  virtual ~AsymmetricEncryptor() {}
    
  // APIs
  virtual std::unique_ptr<AsymmetricKey> Create(KeyBits key_bits) = 0;

  virtual bool PublicKeyEncrypt(std::unique_ptr<AsymmetricKey>& key,
                                const InputStream* plaintext,
                                OutputStream* output) = 0;
  virtual bool PrivateKeyDecrypt(std::unique_ptr<AsymmetricKey>& key,
                                 const InputStream*  cipher,
                                 OutputStream* output) = 0;

  virtual bool Sign(std::unique_ptr<AsymmetricKey>& key,
                    const InputStream* data,
                    const Byte* sign_id, int sign_id_len, // TODO
                    OutputStream* signature) = 0;

  virtual bool Verify(std::unique_ptr<AsymmetricKey>& key,
                      const InputStream* data,
                      const InputStream* signature,
                      const Byte* sign_id, int sign_id_len) = 0; // TODO
};

} // namespace crypto
#endif // CRYPTO_ASYMMETRIC_ENCRYPTO_H_
