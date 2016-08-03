#include "crypto/symmetric_encryptor.h"
#include "crypto/symmetric_crypt.h"

namespace crypto {

SymmetricEncryptor::SymmetricEncryptor(SymmetricKey* key) 
  : key_(key) {
  DCHECK(key_);
}

SymmetricEncryptor::SymmetricEncryptor(SymmetricKey* key,
                                       SymmetricCrypt* crypt)
  : key_(key),
    crypt_(crypt){
  DCHECK(key_);
  DCHECK(crypt_);
}

bool SymmetricEncryptor::Encrypt(const base::StringPiece& plaintext,
                                 std::string* ciphertext) {
  DCHECK(key_);
  DCHECK(crypt_);
  return crypt_->DoEncrypt(this, plaintext, ciphertext);
}

bool SymmetricEncryptor::Decrypt(const base::StringPiece& ciphertext,
                                 std::string* plaintext) {
  DCHECK(key_);
  DCHECK(crypt_);
  return crypt_->DoDecrypt(this, ciphertext, plaintext);
}

} // namespace crypto
