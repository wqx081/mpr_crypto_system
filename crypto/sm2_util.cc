//@Author: WangQixiang (wangqx at mpreader)
//@Date: 2016/08/25

#include "crypto/sm2_util.h"
#include "third_party/libsm2/sm2/native_sm2.h"

#include <string>
#include <glog/logging.h>

namespace crypto {

namespace {

static inline size_t BitsToBytes(size_t bits) {
  return bits / 8;
}

static inline size_t BytesToBits(size_t bytes) {
  return bytes * 8;
}

const char* ErrorNumberToString(int errnum) {
  switch (errnum) {
    case SDR_UNKNOWERR:     return "SDR_UNKNOWERR";
    case SDR_NOTSUPPORT:    return "SDR_NOTSUPPORT";
    case SDR_COMMFAIL:      return "SDR_COMMFAIL";
    case SDR_HARDFAIL:      return "SDR_HARDFAIL";
    case SDR_OPENDEVICE:    return "SDR_OPENDEVICE";
    case SDR_OPENSESSION:   return "SDR_OPENSESSION";
    case SDR_PARDENY:       return "SDR_PARDENY";
    case SDR_KEYNOTEXIST:   return "SDR_KEYNOTEXIST";
    case SDR_ALGNOTSUPPORT: return "SDR_ALGNOTSUPPORT";
    case SDR_PKOPERR:       return "SDR_PKOPERR";
    case SDR_SKOPERR:       return "SDR_SKOPERR";
    case SDR_VERIFYERR:     return "SDR_VERIFYERR";
    case SDR_SYMOPERR:      return "SDR_SYMOPERR";
    case SDR_STEPERR:       return "SDR_STEPERR";
    case SDR_FILESIZEERR:   return "SDR_FILESIZEERR";
    case SDR_FILENOEXIST:   return "SDR_FILENOEXIST";
    case SDR_FILEOFSERR:    return "SDR_FILEOFSERR";
    case SDR_KEYTYPEERR:    return "SDR_KEYTYPEERR";
    case SDR_KEYERR:        return "SDR_KEYERR";
    case SDR_DATASIZEERR:   return "SDR_DATASIZEERR";
    case SDR_FILEOPERR:     return "SDR_FILEOPERR";
    case SDR_MEMERR:        return "SDR_MEMERR";
    case SDR_PARAMERR:      return "SDR_PARAMERR";
    default:    return "Unknown";
  }
}

} // namespace

base::Status Sm2Util::GenerateKeyPair(int key_bits,
                                      uint8_t* public_key,
                                      int* public_key_len,
                                      uint8_t* private_key,
                                      int* private_key_len) {
  int ret = 0;

  if (key_bits != 192 && key_bits != 256) {
    return base::Status(base::Code::INVALID_ARGUMENT, "key bits must be 192 or 256");
  }

  if ((ret = SDF_GenerateKeyPair_ECC(key_bits,
                              (char *)public_key, public_key_len,
                              (char *)private_key, private_key_len)) != SDR_OK) {
    LOG(ERROR) << "SDF_GenerateKeyPair_ECC: return: " << ErrorNumberToString(ret);
    return base::Status(base::Code::INTERNAL,
                  std::string("SDF_GenerateKeyPair_ECC: internal error: ") 
                        + ErrorNumberToString(ret));
  }
                              
  return base::Status::OK();
} 

base::Status Sm2Util::PublicEncrypt(const uint8_t* public_key,
                                    int public_key_len,
                                    const uint8_t* plaintext, int plaintext_len,
                                    uint8_t* cipher, int* cipher_len) {
  // DCHECK(len == 48 || len == 64) 
  if (public_key_len != 48 && public_key_len != 64) {
    return base::Status(base::Code::INVALID_ARGUMENT, "public key len invalid");
  }
  
  int ret = 0;
  CipherImpl cipher_impl;
  PublicKeyImpl pub_key_impl = PublicKeyImplFromBuffer(public_key, public_key_len);
  ret = SNF_ExternalEncrypt_ECC(nullptr,
                                SGD_SM2_3,
                                &pub_key_impl,
                                (uint8_t *)plaintext,
                                plaintext_len,
                                &cipher_impl);
  if (ret != SDR_OK) {
    LOG(ERROR) << "SNF_ExternalEncrypt_ECC: return: " << ErrorNumberToString(ret);
    return base::Status(base::Code::INTERNAL,
                  std::string("SDF_ExternalEncrypt_ECC: internal error: ") 
                        + ErrorNumberToString(ret));
  }

  if (!CipherToBuffer(cipher_impl, cipher, cipher_len)) {
    return base::Status(base::Code::INTERNAL,
                  std::string("Cipher To Buffer error"));
  }

  return base::Status::OK();
}

base::Status
Sm2Util::PrivateDecrypt(const uint8_t* private_key,
                        int private_key_len,
                        const uint8_t* cipher, int cipher_len,
                        uint8_t* plaintext, int* plaintext_len) {

  // DCHECK(len == 48 || len == 64) 
  if (private_key_len != 24 && private_key_len != 32) {
    return base::Status(base::Code::INVALID_ARGUMENT, "private key len invalid");
  }

  int ret;
  PrivateKeyImpl prv_key_impl;
  
  prv_key_impl = PrivateKeyImplFromBuffer(private_key, private_key_len);
  CipherImpl cipher_impl =  CipherFromBuffer((unsigned char *)cipher, 
                                             cipher_len, 
                                             prv_key_impl.bits);
  ret = SNF_ExternalDecrypt_ECC(nullptr,
                                SGD_SM2_3,
                                &prv_key_impl,
                                &cipher_impl,
                                plaintext,
                                (uint32_t *)plaintext_len);
  if (ret != SDR_OK) {
    LOG(ERROR) << "SNF_ExternalDecrypt_ECC: return: " << ErrorNumberToString(ret);
    return base::Status(base::Code::INTERNAL,
                  std::string("SNF_ExternalDecrypt_ECC: internal error: ") 
                        + ErrorNumberToString(ret));
  } 

  return base::Status::OK();
}

base::Status
Sm2Util::Sign(const uint8_t* private_key, int private_key_len,
              const uint8_t* data, int data_len,
              const uint8_t* sign_id, int sign_id_len,
              uint8_t* signature, int* signature_len) {

  // DCHECK(len == 48 || len == 64) 
  if (private_key_len != 24 && private_key_len != 32) {
    return base::Status(base::Code::INVALID_ARGUMENT, "private key len invalid");
  }

  PrivateKeyImpl prv_key_impl = PrivateKeyImplFromBuffer(private_key, private_key_len);
  SignatureImpl signature_impl;

  int ret = 0;
  ret = SNF_ExternalSign_ECC(nullptr,
                             SGD_SM2_1,
                             &prv_key_impl,
                             (unsigned char *)sign_id,
                             sign_id_len,
                             (unsigned char *)data, data_len, &signature_impl);
  if (ret != SDR_OK) {
    return base::Status(base::Code::INTERNAL,
                  std::string("SNF_ExternalSign_ECC: internal error: ") 
                        + ErrorNumberToString(ret));
  } 
  
  SignatureToBuffer(signature_impl, signature, signature_len);
  return base::Status::OK();
}

base::Status 
Sm2Util::Verify(const uint8_t* public_key, int public_key_len,
                const uint8_t* data, int data_len,
                const uint8_t* sign_id, int sign_id_len,
                uint8_t* signature, int signature_len) {
  int ret = 0;
  // DCHECK(len == 48 || len == 64) 
  if (public_key_len != 48 && public_key_len != 64) {
    return base::Status(base::Code::INVALID_ARGUMENT, "public key len invalid");
  }
  
  PublicKeyImpl pub_key_impl = PublicKeyImplFromBuffer(public_key, public_key_len);
  if (signature_len < (int)BitsToBytes(pub_key_impl.bits) * 2) {
    return base::Status(base::Code::INVALID_ARGUMENT, "signature parameter invalid");
  }

  SignatureImpl signature_impl = SignatureFromBuffer(signature, pub_key_impl.bits);
  ret = SNF_ExternalVerify_ECC(nullptr,
                               SGD_SM2_1,
                               &pub_key_impl,
                               (unsigned char *) sign_id, sign_id_len,
                               (unsigned char *) data, data_len,
                               &signature_impl);
  if (ret != SDR_OK) {
    return base::Status(base::Code::INTERNAL,
                  std::string("SNF_ExternalVerify_ECC: internal error: ") 
                        + ErrorNumberToString(ret));
  } 
  return base::Status::OK();
}



// Helpers

Sm2Util::PublicKeyImpl 
Sm2Util::PublicKeyImplFromBuffer(const uint8_t* buffer, int len) {
  // DCHECK(len == 48 || len == 64) 
  PublicKeyImpl impl;
  ::memset(&impl, 0, sizeof(impl));
  impl.bits = (BytesToBits(len)) / 2; 
  memcpy(impl.x, buffer, len / 2);
  memcpy(impl.y, buffer + len / 2, len / 2);
  return impl;
}

Sm2Util::PrivateKeyImpl
Sm2Util::PrivateKeyImplFromBuffer(const uint8_t* buffer, int len) {
  // DCHECK(len == 24 || len == 32) 
  PrivateKeyImpl impl;
  memset(&impl, 0, sizeof(impl));
  impl.bits = BytesToBits(len);
  memcpy(impl.D, buffer, len);
  return impl;
}

int Sm2Util::CipherLength(const CipherImpl& cipher) {
  return (BitsToBytes(cipher.bits) * 2) // x & y
       + sizeof(cipher.C)               // C
       + BitsToBytes(cipher.mbits);
}

bool Sm2Util::CipherToBuffer(const CipherImpl& cipher, uint8_t* buffer, int* len) {
  int offset = 0;
  if (*len < CipherLength(cipher)) {
    return false;
  } 

  memcpy(buffer + offset, cipher.x, BitsToBytes(cipher.bits)); 
  offset += BitsToBytes(cipher.bits);
  memcpy(buffer + offset, cipher.y, BitsToBytes(cipher.bits));
  offset += BitsToBytes(cipher.bits);
  memcpy(buffer + offset, cipher.C, sizeof(cipher.C));
  offset += sizeof(cipher.C);
  memcpy(buffer + offset, cipher.M, BitsToBytes(cipher.mbits));
  offset += BitsToBytes(cipher.mbits);

  return true;
}

Sm2Util::CipherImpl
Sm2Util::CipherFromBuffer(uint8_t* buffer, int len, int bits) {
  CipherImpl impl;
  int offset = 0;

  memset(&impl, 0, sizeof(impl));

  impl.bits = bits;
  impl.mbits = (len - impl.bits / 4) - sizeof(impl.C) * 8;
  memcpy(impl.x, buffer + offset, BitsToBytes(impl.bits));
  offset += BitsToBytes(impl.bits);

  memcpy(impl.y, buffer + offset, BitsToBytes(impl.bits));
  offset += BitsToBytes(impl.bits);

  memcpy(impl.C, buffer + offset, sizeof(impl.C));
  offset += sizeof(impl.C);

  memcpy(impl.M, buffer + offset, BitsToBytes(impl.mbits));
  offset += BitsToBytes(impl.mbits);

  return impl;
}

void Sm2Util::SignatureToBuffer(const SignatureImpl& impl,
                                uint8_t* buffer,
                                int* len) {
  *len = impl.bits / 4;
  memcpy(buffer, impl.r, *len / 2);
  memcpy(buffer + *len / 2, impl.s, *len / 2);
}

Sm2Util::SignatureImpl
Sm2Util::SignatureFromBuffer(uint8_t* buffer, int bits) {
  SignatureImpl impl;

  memset(&impl, 0, sizeof(impl));

  impl.bits = bits;
  memcpy(impl.r, buffer, BitsToBytes(bits));
  memcpy(impl.s, buffer + BitsToBytes(bits), BitsToBytes(bits));
  return impl;
}

} // namespace crypto
