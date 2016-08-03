#ifndef CRYPTO_SYMMETRIC_ENCRYPTOR_H_
#define CRYPTO_SYMMETRIC_ENCRYPTOR_H_

#include "base/string_piece.h"
#include "crypto/symmetric_key.h"

namespace crypto {

class SymmetricCrypt;

class Encryptor {
 public:
  virtual std::string GetKey() = 0;
};

class SymmetricEncryptor : public Encryptor {
 public:
  explicit SymmetricEncryptor(SymmetricKey* key);
  explicit SymmetricEncryptor(SymmetricKey* key, SymmetricCrypt* crypt);

  virtual ~SymmetricEncryptor() {}
  
  std::string GetKey() override {
    return key_->key();
  } 
  
  bool Encrypt(const base::StringPiece& plaintext, std::string* ciphertext);
  bool Decrypt(const base::StringPiece& ciphertext, std::string* plaintext);

  void SetCrypt(SymmetricCrypt* new_crypt) {
    crypt_ = new_crypt;
  }

 private:
  SymmetricKey* key_;  
  SymmetricCrypt* crypt_;
};

} // namespace crypto
#endif // CRYPTO_SYMMETRIC_ENCRYPTOR_H_
