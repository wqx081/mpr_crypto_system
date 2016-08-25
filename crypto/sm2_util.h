//@Author: WangQixiang (wangqx at mpreader.com)
//@Date: 2016/08/25
//
//注意：不要直接使用这里实现的接口, 而是应该使用建立在它之上的接口.
//原因: 0) 非线程安全
//      1) 直接依赖奥联代码库
#ifndef CRYPTO_SM2_UTIL_H_
#define CRYPTO_SM2_UTIL_H_

#include <stdint.h>
#include "base/status.h"
#include "third_party/libsm2/sm2/native_sm2.h"

namespace crypto {

class Sm2Util {
 public:
  
  // Helpers
  using PublicKeyImpl = ECCrefPublicKey;
  using PrivateKeyImpl = ECCrefPrivateKey;
  using CipherImpl = ECCCipherEx;
  using SignatureImpl = ECCSignatureEx;

  static PublicKeyImpl PublicKeyImplFromBuffer(const uint8_t* buffer, int len);
  static PrivateKeyImpl PrivateKeyImplFromBuffer(const uint8_t* buffer, int len);
   
  static int CipherLength(const CipherImpl& cipher);
  static bool CipherToBuffer(const CipherImpl& impl, uint8_t* buffer, int* len);
  static CipherImpl CipherFromBuffer(uint8_t* buffer, int len, int bits);

  static void SignatureToBuffer(const SignatureImpl& signature, uint8_t* buffer, int *len);
  static SignatureImpl SignatureFromBuffer(uint8_t* buffer, int bits);

////// APIs

  //@param key_bits 必须等于 192 or 256
  //@param public_key_len >= 0x41 (65)
  //@param private_key_len >= 0x20 (32)
  static base::Status GenerateKeyPair(int key_bits,
                                      uint8_t* public_key,  int* public_key_len,
                                      uint8_t* private_key, int* private_key_len);

  static base::Status PublicEncrypt(const uint8_t* public_key, 
                                    int public_key_len,
                                    const uint8_t* plaintext, 
                                    int plaintext_len,
                                    uint8_t* cipher, int* cipher_len);

  static base::Status PrivateDecrypt(const uint8_t* private_key, 
                                     int private_key_len,
                                     const uint8_t* cipher, int cipher_len,
                                     uint8_t* plaintext, int* plaintext_len);

  static base::Status Sign(const uint8_t* private_key, int private_key_len,
                           const uint8_t* data, int data_len,
                           const uint8_t* sign_id, int sign_id_len,
                           uint8_t* signature, int* signature_len);

  static base::Status Verify(const uint8_t* public_key, int public_key_len,
                             const uint8_t* data, int data_len,
                             const uint8_t* sign_id, int sign_id_len,
                             uint8_t* signature, int signature_len);
};

} // namespace crypto
#endif // CRYPTO_SM2_UTIL_H_
