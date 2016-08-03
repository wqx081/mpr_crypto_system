#ifndef CRYPTO_SYMETRIC_KEY_H_
#define CRYPTO_SYMETRIC_KEY_H_
#include <stddef.h>
#include <memory>
#include <string>

#include "base/macros.h"

namespace crypto {

class SymmetricKey {
 public:
  enum Algorithm {
    AES,
    HMAC_SHA1,
  };
  virtual ~SymmetricKey();
  static std::unique_ptr<SymmetricKey> GenerateRandomKey(Algorithm algorithm,
                                                      size_t key_size_in_bits);
  static std::unique_ptr<SymmetricKey> DeriveKeyFromPassword(Algorithm algorithm,
                                                             const std::string& password,
                                                             const std::string& salt,
                                                             size_t iterations,
                                                             size_t key_size_in_bits);
  static std::unique_ptr<SymmetricKey> Import(Algorithm algorithm,
                                              const std::string& raw_key);
  static std::unique_ptr<SymmetricKey> FromHexString(const std::string& hex_string);

  std::string ToHexString() const;
  const std::string& key() { return key_; }
  bool GetRawKey(std::string* raw_key);

 private:
  SymmetricKey();

  std::string key_;

  DISALLOW_COPY_AND_ASSIGN(SymmetricKey);
};

} // namespace crypto
#endif // CRYPTO_SYMETRIC_KEY_H_
