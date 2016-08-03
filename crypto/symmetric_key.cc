#include "crypto/symmetric_key.h"

#include "third_party/boringssl/include/openssl/evp.h"
#include "third_party/boringssl/include/openssl/rand.h"
#include <stddef.h>
#include <stdint.h>

#include <algorithm>
#include <utility>

#include <glog/logging.h>
#include "base/string_util.h"
#include "base/string_encode.h"
#include "crypto/openssl_util.h"

namespace crypto {

SymmetricKey::~SymmetricKey() {
  std::fill(key_.begin(), key_.end(), '\0');
}

// static
std::unique_ptr<SymmetricKey> SymmetricKey::GenerateRandomKey(
    Algorithm algorithm,
    size_t key_size_in_bits) {

  DCHECK_EQ(AES, algorithm);
  (void)algorithm;
  
  if (key_size_in_bits != 128 && key_size_in_bits != 256) {
    return nullptr;
  }
  size_t key_size_in_bytes = key_size_in_bits / 8;
  DCHECK_EQ(key_size_in_bits, key_size_in_bytes * 8);

  if (key_size_in_bytes == 0) {
    return nullptr;
  }

  OpenSSLErrStackTracer err_tracer(FROM_HERE);
  std::unique_ptr<SymmetricKey> key(new SymmetricKey);
  uint8_t* key_data = reinterpret_cast<uint8_t*>(
    base::WriteInto(&key->key_, key_size_in_bytes + 1));
  int rv = RAND_bytes(key_data, static_cast<int>(key_size_in_bytes));
  return rv == 1 ? std::move(key) : nullptr;
}

// static
std::unique_ptr<SymmetricKey> SymmetricKey::DeriveKeyFromPassword(
    Algorithm algorithm,
    const std::string& password,
    const std::string& salt,
    size_t iterations,
    size_t key_size_in_bits) {

  DCHECK(algorithm == AES || algorithm == HMAC_SHA1);

  if (algorithm == AES) {
    if (key_size_in_bits != 128 && key_size_in_bits != 256) {
      return nullptr;
    }
  }

  size_t key_size_in_bytes = key_size_in_bits / 8;
  DCHECK_EQ(key_size_in_bits, key_size_in_bytes * 8);

  if (key_size_in_bytes == 0) {
    return nullptr;
  }

  OpenSSLErrStackTracer err_tracer(FROM_HERE);
  std::unique_ptr<SymmetricKey> key(new SymmetricKey);
  uint8_t* key_data = reinterpret_cast<uint8_t *>(
    base::WriteInto(&key->key_, key_size_in_bytes + 1));
  int rv = PKCS5_PBKDF2_HMAC_SHA1(
      password.data(),
      password.length(),
      reinterpret_cast<const uint8_t *>(salt.data()), 
      salt.length(),
      static_cast<unsigned>(iterations),
      key_size_in_bytes, 
      key_data);
  return rv == 1 ? std::move(key) : nullptr;
}

// static
std::unique_ptr<SymmetricKey> SymmetricKey::Import(
    Algorithm algorithm,
    const std::string& raw_key) {
  if (algorithm == AES) {
    if (raw_key.size() != 128/8 && raw_key.size() != 256 / 8) {
      return nullptr;
    }
  }

  std::unique_ptr<SymmetricKey> key(new SymmetricKey);
  key->key_ = raw_key;
  return key;
}

// static
std::unique_ptr<SymmetricKey> SymmetricKey::FromHexString(
    const std::string& hex_string) {
  std::string deoced_hex_string = base::HexDecode(hex_string);
  if (deoced_hex_string.empty()) {
    return nullptr;
  }
  
  std::unique_ptr<SymmetricKey> key(new SymmetricKey);
  key->key_ = deoced_hex_string;
  return key;
}

std::string SymmetricKey::ToHexString() const {
  return base::HexEncode(key_);
}

bool SymmetricKey::GetRawKey(std::string* raw_key) {
  *raw_key = key_;
  return true;
}

SymmetricKey::SymmetricKey() = default;

} // namespace crypto
