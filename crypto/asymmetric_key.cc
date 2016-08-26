#include "crypto/asymmetric_key.h"

namespace crypto {

Sm2Key::Sm2Key(uint8_t* public_key, int public_key_len,
               uint8_t* private_key, int private_key_len) {
  for (int i=0; i < public_key_len; ++i) {
    public_key_.push_back(public_key[i]);
  }
  for (int i=0; i < private_key_len; ++i) {
    private_key_.push_back(private_key[i]);
  }
}

// static
std::unique_ptr<AsymmetricKey> 
Sm2Key::FromBuffer(uint8_t* public_key, int public_key_len,
                   uint8_t* private_key, int private_key_len) {
  return std::unique_ptr<Sm2Key>(new Sm2Key(public_key, public_key_len,
                                            private_key, private_key_len));
}

bool Sm2Key::ExportPublicKey(std::vector<uint8_t>* output) const {
  output->assign(public_key_.data(), public_key_.data() + public_key_.size());
  return true;
}

bool Sm2Key::ExportPrivateKey(std::vector<uint8_t>* output) const {
  output->assign(private_key_.data(), private_key_.data() + private_key_.size());
  return true;
}

} // namespace crypto
