#include "crypto/sm2_util.h"
#include "base/status.h"

#include <thread>
#include <vector>

#include <gtest/gtest.h>

namespace crypto {

const uint32_t kKey256Bits = 256;
const uint32_t kKey192Bits = 192;

class GenerateKeyPairWorker {
 public:
  void operator()() {
    uint8_t pub_key[1024] = {0};  
    int pub_key_len = 1024;
    uint8_t prv_key[1024] = {0};  
    int prv_key_len = 1024;
    EXPECT_TRUE(Sm2Util::GenerateKeyPair(kKey256Bits,
                                         pub_key, &pub_key_len,
                                         prv_key, &prv_key_len).ok());    
    
  }
};

TEST(Sm2UtilTest, GenerateKeyPair) {
  uint8_t pub_key[1024] = {0};  
  int pub_key_len = 1024;
  uint8_t prv_key[1024] = {0};  
  int prv_key_len = 128;

  EXPECT_TRUE(Sm2Util::GenerateKeyPair(kKey256Bits,
                                       pub_key, &pub_key_len,
                                       prv_key, &prv_key_len).ok());    
  LOG(INFO) << "kKey256Bits pub_key_len: " << pub_key_len 
                                           << "prv_key_len: " << prv_key_len;
  pub_key_len = prv_key_len = 1024;
  EXPECT_TRUE(Sm2Util::GenerateKeyPair(kKey192Bits,
                                       pub_key, &pub_key_len,
                                       prv_key, &prv_key_len).ok());    
  LOG(INFO) << "kKey192Bits pub_key_len: " << pub_key_len 
                                           << "prv_key_len: " << prv_key_len;
}

#if 0
TEST(Sm2UtilTest, MultiThreadKeyPair) {
  std::vector<std::thread> thread_pool;
  for (int i=0; i < 10; ++i) {
    thread_pool.push_back(std::thread(GenerateKeyPairWorker()));
  }
  for (int i=0; i < 10; ++i) {
    thread_pool[i].join();
  }
}
#endif

} // namespace crypto
