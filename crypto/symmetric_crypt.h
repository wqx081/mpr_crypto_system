#ifndef CRYPTO_SYMMETRIC_CRYPT_H_
#define CRYPTO_SYMMETRIC_CRYPT_H_

#include "base/string_piece.h"
#include "crypto/symmetric_key.h"
#include "crypto/symmetric_encryptor.h"

namespace crypto {

//////// Strategy Pattern
class SymmetricCrypt {
 public:
  virtual ~SymmetricCrypt() {}

  virtual bool DoEncrypt(Encryptor* encryptor, 
                         const base::StringPiece& plaintext,
                         std::string* ciphertext) = 0;
  virtual bool DoDecrypt(Encryptor* encryptor,
                         const base::StringPiece& ciphertext,
                         std::string* plaintext) = 0;
};

class CBCSymmetricCrypt : public SymmetricCrypt {
 public:
  explicit CBCSymmetricCrypt(const base::StringPiece& iv);
  bool DoEncrypt(Encryptor*, const base::StringPiece&, std::string*) override;
  bool DoDecrypt(Encryptor*, const base::StringPiece&, std::string*) override;
 private:
  std::string iv_;
};

class ECBSymmetricCrypt : public SymmetricCrypt {
 public:
  ECBSymmetricCrypt() {}
  bool DoEncrypt(Encryptor*, const base::StringPiece&, std::string*) override;
  bool DoDecrypt(Encryptor*, const base::StringPiece&, std::string*) override;
};

class CTRSymmetricCrypt : public SymmetricCrypt {
 public:
  CTRSymmetricCrypt() {}

  bool DoEncrypt(Encryptor*, const base::StringPiece&, std::string*) override;
  bool DoDecrypt(Encryptor*, const base::StringPiece&, std::string*) override;

  class Counter {
   public:
    explicit Counter(const base::StringPiece& counter);
    ~Counter();
    bool Increment();
    void Write(void* buf);
    size_t GetLengthInBytes() const;

   private:
    union {
      uint32_t components32[4];
      uint64_t components64[2];
    } counter_;
  };

  bool SetCounter(const base::StringPiece& counter);
 private:
  std::unique_ptr<Counter> counter_;  
  bool GenerateCounterMask(size_t plaintext_len,
                           uint8_t* mask,
                           size_t* mask_len);
  void MaskMessage(const void* plaintext,
                   size_t plaintext_len,
                   const void* mask,
                   void* ciphertext) const;
  bool Crypt(bool do_encrypt,
             const std::string& raw_key,
             const base::StringPiece& input,
             std::string* output);  
};

} // namespace crypto
#endif // CRYPTO_SYMMETRIC_CRYPT_H_
