#include "crypto/sm2_access_point.h"
#include "crypto/sm2_util.h"
#include "crypto/asymmetric_key.h"
#include "crypto/asymmetric_padding.h"
#include "base/status.h"

#include "third_party/libsm2/MPRAsymmCrypt.h"

namespace crypto {

const int Sm2AccessPoint::kSm2BlockSize;

Sm2AccessPoint::Sm2AccessPoint()
  : padding_strategy_(new ZeroPadding) {}

// static
Sm2AccessPoint* Sm2AccessPoint::GetInstance() {
  static Sm2AccessPoint instance_;
  return &instance_;
}

// static
std::unique_ptr<AsymmetricKey> Sm2AccessPoint::Create(KeyBits key_bits) {
  //LOCK
  std::lock_guard<std::mutex> lock(access_mutex_);

  uint8_t public_key[256] = {0};  
  uint8_t private_key[256] = {0};  
  int public_key_len = 256;
  int private_key_len = 256;
  
  int key_bits_impl;
  if ((key_bits_impl = KeyBitsToInternal(key_bits)) == kKeyBitsNone) {
    //LOG(ERROR)
    return nullptr;
  }    

  base::Status status = Sm2Util::GenerateKeyPair(key_bits_impl, public_key, &public_key_len,
                                                 private_key, &private_key_len);
  if (!status.ok()) {
    //LOG(ERROR)
    return nullptr;
  }

  return Sm2Key::FromBuffer(public_key, public_key_len, private_key, private_key_len);
}

bool Sm2AccessPoint::PublickKeyEncrypt(const std::vector<uint8_t>& public_key,
                                       const base::StringPiece& plaintext,
                                       std::string* output) {
  //LOCK
  std::lock_guard<std::mutex> lock(access_mutex_);
  
  if (plaintext.size() == 0) {
    return true;
  }
  output->clear();

  base::Status status; 
  std::vector<std::array<unsigned char, 128>> result;
  int cipher_len = 128;
  std::array<unsigned char, 128> cipher;                                    
  std::string block;                                     
  size_t count = plaintext.size() / kSm2BlockSize;
  size_t extra = plaintext.size() % kSm2BlockSize;
  size_t index = 0;

  while (count > 0) {
    block = plaintext.substr(index, kSm2BlockSize).as_string();
    status = Sm2Util::PublicEncrypt(public_key.data(), public_key.size(),
                                    (const unsigned char*)(block.data()), kSm2BlockSize,
                                    cipher.data(), &cipher_len);
    if (!status.ok()) {
      LOG(INFO) << "-----1" << status.ToString();
      return false;
    }

    result.push_back(cipher);

    index += kSm2BlockSize;
    count--;
  } 

  // 处理最后一块
  if (extra == 0) { // 正好为 kSm2Block 整数倍
    block="";
    padding_strategy_->FillPadding(block, kSm2BlockSize);
    if (!block.empty()) {
      status = Sm2Util::PublicEncrypt(public_key.data(), public_key.size(),
                                      (const unsigned char*)(block.data()), kSm2BlockSize,
                                      cipher.data(), &cipher_len);
      if (!status.ok()) {
      LOG(INFO) << "-----2";
        return false;
      }
      result.push_back(cipher);
    }
  } else {
    block = plaintext.substr(index, extra).as_string();
    padding_strategy_->FillPadding(block, kSm2BlockSize);
    status = Sm2Util::PublicEncrypt(public_key.data(), public_key.size(),
                                    (const unsigned char*)(block.data()), kSm2BlockSize,
                                    cipher.data(), &cipher_len);
    if (!status.ok()) {
      LOG(INFO) << "-----2";
      return false;
    }
    result.push_back(cipher);
  }

  std::string str;
  for (size_t i = 0; i < result.size(); ++i) {
    str += std::string((char *)result[i].data(), result[i].size());
  } 
  
  output->swap(str);
  return true;
}

bool Sm2AccessPoint::PrivateKeyDecrypt(const std::vector<uint8_t>& private_key,
                                       const base::StringPiece& cipher,
                                       std::string* output) {
  //LOCK
  std::lock_guard<std::mutex> lock(access_mutex_);
  if (cipher.empty() || private_key.empty()) {
    return false;
  } 
  output->clear();

  base::Status status;
  std::vector<std::array<unsigned char, 128>> result;
  int plaintext_len = 128;
  std::array<unsigned char, 128> plaintext;
  size_t count = cipher.size() / 128;
  size_t index = 0;

  while (count > 0) {
    //block = cipher.substr(index, kSm2BlockSize).as_string();
    LOG(INFO) << "----x";
#if 0
    status = Sm2Util::PrivateDecrypt(private_key.data(), private_key.size(),
                                     (const uint8_t *)(cipher.data() + index),
                                     128,
                                     plaintext.data(), &plaintext_len);
    
    LOG(INFO) << "----y";
    if (!status.ok()) {
      LOG(ERROR) << "PrivateDecrypt Error: " << status.ToString();
      return false;
    }
#endif
   DCHECK_EQ(MPRAsymmCrypt_PrvDecrypt(SM2, 
                                   private_key.data(), (unsigned int)private_key.size(),
                                   (unsigned char *)(cipher.data() + index), 128,
                                   plaintext.data(), (unsigned int *)&plaintext_len) , 0);

    result.push_back(plaintext);

    index += kSm2BlockSize;
    count--;
  }

  std::string str;
  for (index = 0; index < result.size() - 1; ++index) {
    str += std::string((char*)result[index].data(), result[index].size());
  }
  // Trim the last one block Padding
  std::string block;
  block = std::string((char*)result[index + 1].data(), result[index + 1].size()); 
  padding_strategy_->TrimPadding(block, 128);
  str += block;

  output->swap(str);
  return true;
}

void Sm2AccessPoint::set_padding_strategy(std::shared_ptr<PaddingInterface> v) {
  //LOCK
  std::lock_guard<std::mutex> lock(access_mutex_);
  padding_strategy_ = v; 
}

} // namespace crypto
