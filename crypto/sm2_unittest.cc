#include "third_party/libsm2/MPRAsymmCrypt.h"

#include <gtest/gtest.h>
#include "base/string_encode.h"
#include "base/string_piece.h"
#include "base/file.h"
#include "base/file_path.h"
#include "base/file_util.h"

#include <string>

namespace base {

const char* kM3u8Path = "/tmp/wqx/test/test.m3u8";
const char* kMediaKeyPath = "#EXT-X-KEY:METHOD=AES-128,URI=\"http://172.16.2.129:80/media/enc/video.key\"0";


class AsymmEncryptStrage {
 public:
  static const int kPaddingBlockSize = 32;

  static void Padding(std::string& input) {
    if (input.size() < kPaddingBlockSize) {
      // padding
      input.insert(input.size(), kPaddingBlockSize - input.size(), 0);
    }  
  }
  
  static bool PublicEncrypt(unsigned char* public_key, 
                     unsigned int public_key_len, 
                     const std::string& input,
                     std::vector<std::array<unsigned char, 128>>& output) {
    for (size_t i = 0; i < input.size(); i += kPaddingBlockSize) {
      int substr_len = input.size() - i >= kPaddingBlockSize ? kPaddingBlockSize : input.size() - i;
      std::string block = input.substr(i, substr_len);
      Padding(block);
//      LOG(INFO) << "input_str: " << block << ", input_len: " << block.size();
      unsigned int cipher_len = 128;
      std::array<unsigned char, 128> cipher;  
      EXPECT_EQ(0, MPRAsymmCrypt_PubEncrypt(SM2, public_key, public_key_len,
                               (const unsigned char *) (block.data()), 
                               kPaddingBlockSize,
                               cipher.data(), 
                               &cipher_len));
      output.push_back(cipher);
    }
    return true;
  }
                       
};

const int AsymmEncryptStrage::kPaddingBlockSize;

class ScopedAsymmCrypt {
 public:
  ScopedAsymmCrypt() { DCHECK(MPRAsymmCrypt_Init() == 0); }
  ~ScopedAsymmCrypt() { DCHECK(MPRAsymmCrypt_Uninit() == 0); }
};

TEST(SM2, FileEncrypt) {
  unsigned char public_key[128] = {0};
  unsigned int public_key_len = 128;
  unsigned char private_key[128] = {0};
  unsigned int private_key_len = 128;

  ScopedAsymmCrypt scoped_asymc_cyrpt;

  EXPECT_EQ(0, MPRAsymmCrypt_GenerateKeyPair(SM2,
                                             256,
                                            public_key, &public_key_len,
                                            private_key, &private_key_len));
  std::string public_key_str = base::HexEncode((const char *)public_key, public_key_len);
  std::string private_key_str = base::HexEncode((const char *)private_key, private_key_len);
  LOG(INFO) << "public_key: " << public_key_str;
  LOG(INFO) << "private_key: " << private_key_str;

  std::vector<std::array<unsigned char, 128>> cipher_result;
  EXPECT_TRUE(AsymmEncryptStrage::PublicEncrypt(public_key, public_key_len, std::string(kMediaKeyPath), cipher_result)); 
  for (auto c : cipher_result) {
    std::string cipher_text = base::HexEncode((const char *)c.data(), c.size());
    LOG(INFO) << "cipher_text: " << cipher_text; 
  }

  //std::vector<std::array<unsigned char, 32>> plain_result;
  for (auto c : cipher_result) {
    unsigned char plaintext[128] = {0};
    unsigned int plaintext_len = 128;
    EXPECT_EQ(0, MPRAsymmCrypt_PrvDecrypt(SM2, private_key, private_key_len,
                                          c.data(), c.size(), plaintext, &plaintext_len));
    std::string text((char *)plaintext);
    LOG(INFO) << "Decrypt : " << text;
  }

  EXPECT_TRUE(true);
}

}
