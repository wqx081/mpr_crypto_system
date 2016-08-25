//@Author: WangQixiang (wangqx at mpreader.com)
//@Date: 2016/08/25
//
#ifndef CRYPTO_ASYMMETRIC_KEY_H_
#define CRYPTO_ASYMMETRIC_KEY_H_
#include "base/macros.h"

#include <vector>
#include <memory>

namespace crypto {

class AsymmetricKey {
 public:
  virtual ~AsymmetricKey() {}
  virtual bool ExportPrivateKey(std::vector<uint8_t>* output) const = 0;
  virtual bool ExportPublicKey(std::vector<uint8_t>* output) const = 0;
};

// kKey256Bits pub_key_len: 64 prv_key_len: 32
// kKey192Bits pub_key_len: 48 prv_key_len: 24
class Sm2Key : public AsymmetricKey {
 public:
  static std::unique_ptr<AsymmetricKey> FromBuffer(uint8_t* public_key, int public_key_len,
                                                   uint8_t* private_key, int private_key_len);
  // AsymmetricKey
  virtual bool ExportPrivateKey(std::vector<uint8_t>* output) const override;
  virtual bool ExportPublicKey(std::vector<uint8_t>* output) const override;

 private:
  Sm2Key(uint8_t* public_key, int public_key_len,
         uint8_t* private_key, int private_key_len);

  std::vector<uint8_t> public_key_;
  std::vector<uint8_t> private_key_;

  DISALLOW_COPY_AND_ASSIGN(Sm2Key);
};

} // namespace crypto
#endif // CRYPTO_ASYMMETRIC_KEY_H_
