#include "crypto/symmetric_crypt.h"

#include "crypto/openssl_util.h"
#include "base/location.h"
#include "base/string_util.h"
#include "base/byteorder.h"

#include <stddef.h>
#include <stdint.h>

#include <memory>
#include <string>

#include "third_party/boringssl/include/openssl/aes.h"
#include "third_party/boringssl/include/openssl/evp.h"

namespace crypto {

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
    ClearOpenSSLERRStack(FROM_HERE);
  }
  EVP_CIPHER_CTX* get() { return &ctx_; }

 private:
  EVP_CIPHER_CTX ctx_;
};

bool Crypt(bool do_encrypt,
           const std::string& raw_key,
           const EVP_CIPHER* cipher,
           const base::StringPiece& input,
           std::string* output,
           const std::string iv="",
           bool do_padding = true) {
  DCHECK(!raw_key.empty());  
  DCHECK(cipher);

  std::string result;
  output->clear();
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
                         reinterpret_cast<const uint8_t *>(raw_key.data()),
                do_cbc ? reinterpret_cast<const uint8_t *>(iv.data()) : nullptr,
                         do_encrypt)) {
    return false;
  }
  
  // When padding && encrypting, add other block size of space to allow for any padding.
  const size_t output_size = input.size() + (do_padding ? (do_encrypt ? AES_BLOCK_SIZE : 0)
                                                        : 0); 
  CHECK_GT(output_size, 0u);
  CHECK_GT(output_size + 1, input.size());
  uint8_t* out_ptr = reinterpret_cast<uint8_t*>(base::WriteInto(&result, output_size + 1));
  int out_len;
  if (!EVP_CipherUpdate(ctx.get(),
                        out_ptr,
                        &out_len,
                        reinterpret_cast<const uint8_t *>(input.data()),
                        input.length())) {
    return false;
  }

  // Write out the final block plus padding (if any) to the end of the data
  // just written.
  int tail_len;
  if (!EVP_CipherFinal_ex(ctx.get(), out_ptr + out_len, &tail_len))
    return false;

  out_len += tail_len;
  DCHECK_LE(out_len, static_cast<int>(output_size));
  result.resize(out_len);

  output->swap(result);
  return true;
}

} // namespace

// CBC (iv)
CBCSymmetricCrypt::CBCSymmetricCrypt(const base::StringPiece& iv) {
  DCHECK(iv.size() == 16u);
  iv.CopyToString(&iv_);
}

bool CBCSymmetricCrypt::DoEncrypt(Encryptor* encryptor,
                                  const base::StringPiece& plaintext,
                                  std::string* ciphertext) {
  std::string key = encryptor->GetKey();
  const EVP_CIPHER* cipher = CBCGetCipherForKey(key);
  DCHECK(cipher != nullptr);
  return Crypt(true,
               key,
               cipher,
               plaintext,
               ciphertext,
               iv_);
}

bool CBCSymmetricCrypt::DoDecrypt(Encryptor* encryptor,
                                  const base::StringPiece& ciphertext,
                                  std::string* plaintext) {
  std::string key = encryptor->GetKey();
  const EVP_CIPHER* cipher = CBCGetCipherForKey(key);
  return Crypt(false,
               key,
               cipher,
               ciphertext,
               plaintext,
               iv_); 
}

// ECB
bool ECBSymmetricCrypt::DoEncrypt(Encryptor* encryptor,
                                  const base::StringPiece& plaintext,
                                  std::string* ciphertext) {
  std::string key = encryptor->GetKey();
  const EVP_CIPHER* cipher = ECBGetCipherForKey(key);
  return Crypt(true,
               key,
               cipher,
               plaintext,
               ciphertext); 
}

bool ECBSymmetricCrypt::DoDecrypt(Encryptor* encryptor,
                                  const base::StringPiece& ciphertext,
                                  std::string* plaintext) {
  std::string key = encryptor->GetKey();
  const EVP_CIPHER* cipher = ECBGetCipherForKey(key);
  return Crypt(false,
               key,
               cipher,
               ciphertext,
               plaintext); 
}


// CTR (counter)
//

bool CTRSymmetricCrypt::DoEncrypt(Encryptor* encryptor,
                                  const base::StringPiece& plaintext,
                                  std::string* ciphertext) {
  std::string key = encryptor->GetKey();
  return Crypt(true,
               key,
               plaintext,
               ciphertext);
}

bool CTRSymmetricCrypt::DoDecrypt(Encryptor* encryptor,
                                  const base::StringPiece& ciphertext,
                                  std::string* plaintext) {
  std::string key = encryptor->GetKey();
  return Crypt(false,
               key,
               ciphertext,
               plaintext);
}

CTRSymmetricCrypt::Counter::Counter(const base::StringPiece& counter) {
  CHECK(sizeof(counter_) == counter.length());
  memcpy(&counter_, counter.data(), sizeof(counter_));
}

CTRSymmetricCrypt::Counter::~Counter() { }

bool CTRSymmetricCrypt::Counter::Increment() {
  uint64_t low_num = base::NetToHost64(counter_.components64[1]);
  uint64_t new_low_num = low_num + 1;
  counter_.components64[1] = base::HostToNet64(new_low_num);

  if (new_low_num < low_num) {
    counter_.components64[0] =
        base::HostToNet64(base::NetToHost64(counter_.components64[0]) + 1);
  }

  return true;
}

void CTRSymmetricCrypt::Counter::Write(void* buf) {
  uint8_t* buf_ptr = reinterpret_cast<uint8_t*>(buf);
  memcpy(buf_ptr, &counter_, sizeof(counter_));
}

size_t CTRSymmetricCrypt::Counter::GetLengthInBytes() const {
  return sizeof(counter_);
}

bool CTRSymmetricCrypt::SetCounter(const base::StringPiece& counter) {
  if (counter.length() != 16u) {
    return false;
  }
  counter_.reset(new Counter(counter));
  return true;
}

bool CTRSymmetricCrypt::GenerateCounterMask(size_t plaintext_len,
    uint8_t* mask,
    size_t* mask_len) {

  CHECK(mask);
  CHECK(mask_len);

  const size_t kBlockLength = counter_->GetLengthInBytes();
  size_t blocks = (plaintext_len + kBlockLength - 1) / kBlockLength;
  CHECK(blocks);

  *mask_len = blocks * kBlockLength;

  for (size_t i = 0; i < blocks; ++i) {
    counter_->Write(mask);
    mask += kBlockLength;

    bool ret = counter_->Increment();
    if (!ret)
      return false;
  }
  return true;
}

void CTRSymmetricCrypt::MaskMessage(const void* plaintext,
                                    size_t plaintext_len,
                                    const void* mask,
                                    void* ciphertext) const {
  const uint8_t* plaintext_ptr = reinterpret_cast<const uint8_t*>(plaintext);
  const uint8_t* mask_ptr = reinterpret_cast<const uint8_t*>(mask);
  uint8_t* ciphertext_ptr = reinterpret_cast<uint8_t*>(ciphertext);

  for (size_t i = 0; i < plaintext_len; ++i) {
    ciphertext_ptr[i] = plaintext_ptr[i] ^ mask_ptr[i];
  }
}

bool CTRSymmetricCrypt::Crypt(bool do_encrypt,
                              const std::string& raw_key,
                              const base::StringPiece& input,
                              std::string* output) {
  (void) do_encrypt;

  if (!counter_.get()) {
    LOG(ERROR) << "Counter value not set in CTR mode.";
    return false;
  }

  AES_KEY aes_key;
  if (AES_set_encrypt_key(reinterpret_cast<const uint8_t*>(raw_key.data()),
                          raw_key.size() * 8, &aes_key) != 0) {
    return false;
  }

  const size_t out_size = input.size();
  CHECK_GT(out_size, 0u);
  CHECK_GT(out_size + 1, input.size());

  std::string result;
  uint8_t* out_ptr =
      reinterpret_cast<uint8_t*>(base::WriteInto(&result, out_size + 1));

  uint8_t ivec[AES_BLOCK_SIZE] = { 0 };
  uint8_t ecount_buf[AES_BLOCK_SIZE] = { 0 };
  unsigned int block_offset = 0;

  counter_->Write(ivec);

  AES_ctr128_encrypt(reinterpret_cast<const uint8_t*>(input.data()), out_ptr,
                     input.size(), &aes_key, ivec, ecount_buf, &block_offset);


  SetCounter(base::StringPiece(reinterpret_cast<const char*>(ivec),
                               AES_BLOCK_SIZE));

  output->swap(result);
  return true;
}


} // namespace crypto
