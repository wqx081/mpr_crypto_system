//@Author: WangQixiang (wangqx at mpreader.com)
//@Date: 2016/08/25
//
// Sm2AccessPoint 实现了线程安全的单例模式, 直接依赖底层的 Sm2Util代码
//
#ifndef CRYPTO_SM2_ACCESS_POINT_H_
#define CRYPTO_SM2_ACCESS_POINT_H_

#include "base/macros.h"
#include "base/once.h"
#include "base/string_piece.h"

#include <utility>
#include <vector>

namespace crypto {

class AsymmetricKey;
class PaddingInterface;

/////////////////////////////////
class Sm2AccessPoint {
 public:
  
  enum KeyBits {
    kKey256Bits = 0,
    kKey192Bits
  };

  static Sm2AccessPoint* GetInstance();

  std::unique_ptr<AsymmetricKey> Create(KeyBits key_bits=kKey192Bits);  

  bool PublickKeyEncrypt(const std::vector<uint8_t>& public_key,
                         const base::StringPiece& plaintext,
                         std::string* output);

  bool PrivateKeyDecrypt(const std::vector<uint8_t>& private_key,
                         const base::StringPiece& cipher,
                         std::string* output);

  void set_padding_strategy(std::shared_ptr<PaddingInterface> v);
  //TODO(wqx): Signature And Veirfy
 private:
  Sm2AccessPoint();
  std::shared_ptr<PaddingInterface> padding_strategy_;
  DISALLOW_COPY_AND_ASSIGN(Sm2AccessPoint);
};

} // namespace crypto
#endif // CRYPTO_SM2_ACCESS_POINT_H_
