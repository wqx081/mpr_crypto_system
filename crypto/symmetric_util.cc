#include "crypto/symmetric_util.h"
#include "crypto/openssl_util.h"

#include "third_party/boringssl/include/openssl/aes.h"
#include "third_party/boringssl/include/openssl/evp.h"

#include "base/location.h"
#include "base/string_util.h"

#include <memory>
#include <string>

#include <glog/logging.h>

namespace {

const EVP_CIPHER* CBCGetCipherForKey(const std::string& key) {
  switch (key.length()) {
    case 16: return EVP_aes_128_cbc();
    case 32: return EVP_aes_256_cbc();
    default:
      return nullptr;
  }
}

const EVP_CIPHER* ECBGetCipherForKey(const std::string& key) {
  switch (key.length()) {
    case 16: return EVP_aes_128_ecb();
    case 32: return EVP_aes_256_ecb();
    default:
      return nullptr;
  }
}

class ScopedCipherCTX {
 public:
  explicit ScopedCipherCTX() {
    EVP_CIPHER_CTX_init(&ctx_);
  }
  ~ScopedCipherCTX() {
    EVP_CIPHER_CTX_cleanup(&ctx_);
    crypto::ClearOpenSSLERRStack(FROM_HERE);
  }
  EVP_CIPHER_CTX* get() { return &ctx_; }

 private:
  EVP_CIPHER_CTX ctx_;
};

} // namespace

namespace crypto {

bool CryptInternal(bool do_encrypt, const std::string& raw_key, 
                   const EVP_CIPHER* cipher, 
                   InputStream* in, OutputStream* out,
                   const std::string& iv="") {
  DCHECK(!raw_key.empty());
  DCHECK(cipher);
  bool do_cbc = false;

  if (!iv.empty() && iv.size() == 16) { // CBC
    do_cbc = true;
    DCHECK_EQ(EVP_CIPHER_iv_length(cipher), iv.length());    
  } else {
    do_cbc = false;
  }

  ScopedCipherCTX ctx;
  if (!EVP_CipherInit_ex(ctx.get(),
                         cipher,
                         nullptr,
                         reinterpret_cast<const uint8_t*>(raw_key.data()),
                do_cbc ? reinterpret_cast<const uint8_t*>(iv.data()) : nullptr,
                         do_encrypt)) {
    return false;
  }

  size_t count = in->Available() / AES_BLOCK_SIZE;
  size_t extra = in->Available() % AES_BLOCK_SIZE;
  
//  LOG(INFO) << "cout: " << count << ", extra: " << extra;

  while (count > 0) {

    ByteVector in_buf;
    int out_len;
    size_t output_size;
    std::string result;

    if (count == 1 && do_encrypt ) { // last one
      in_buf.resize(AES_BLOCK_SIZE + extra);                                     
      output_size = in_buf.size() + AES_BLOCK_SIZE;
    } else {
      in_buf.resize(AES_BLOCK_SIZE);
      output_size = in_buf.size();
    }

    DCHECK(in->Read(&in_buf) == in_buf.size());

    CHECK_GT(output_size, 0u);
    CHECK_GT(output_size + 1, in_buf.size());
    uint8_t* out_ptr = reinterpret_cast<uint8_t*>(base::WriteInto(&result, output_size + 1));

    if (!EVP_CipherUpdate(ctx.get(),
                          out_ptr,
                          &out_len, // output len
                          in_buf.data(),
                          in_buf.size())) {
      DCHECK(false);
      return false;
    }

    int tail_len = 0;
    if (count == 1) {
      if (!EVP_CipherFinal_ex(ctx.get(), out_ptr + out_len, &tail_len)) {
        return false;
      }
    }
    out->Write((Byte *)out_ptr, 0, out_len + tail_len);
    count--;
  }
  return true;
}

bool EcbEncrypt(const std::string& raw_key, InputStream* in, OutputStream* out) {
  return CryptInternal(true, raw_key, ECBGetCipherForKey(raw_key), in, out);
}

bool EcbDecrypt(const std::string& raw_key, InputStream* in, OutputStream* out) {
  return CryptInternal(false, raw_key, ECBGetCipherForKey(raw_key), in, out);
}

bool CbcEncrypt(const std::string& raw_key, const std::string& iv,
                InputStream* in, OutputStream* out) {
  return CryptInternal(true, raw_key, CBCGetCipherForKey(raw_key), in, out, iv);
}

bool CbcDecrypt(const std::string& raw_key, const std::string& iv,
                InputStream* in, OutputStream* out) {
  return CryptInternal(false, raw_key, CBCGetCipherForKey(raw_key), in, out, iv);
}

} // namespace crypto
