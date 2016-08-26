#include "crypto/sm2_access_point.h"
#include "base/status.h"
#include "crypto/asymmetric_key.h"
#include "base/string_encode.h"
#include "base/file_path.h"
#include "base/file_util.h"

#include "base/string_util.h"

#include <thread>
#include <vector>

#include <gtest/gtest.h>

const char* kM3u8Path = "/tmp/test.m3u8";
const char* kEncM3u8Path = "/tmp/enc_test.m3u8";

const char* kMediaKeyPathPrefix = "#EXT-X-KEY:METHOD=AES-128,URI=";
const char* kMediaKeyPath = "#EXT-X-KEY:METHOD=AES-128,URI=\"http://172.16.2.129:80/media/enc/video.key\"";

const char* kVideoKeyPathAnchor= "{video_key_path}";

const int kMaxWorkers = 10;

bool Replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

class GenerateKeyPairWorker {
 public:
  void operator()() {
    std::unique_ptr<crypto::AsymmetricKey> key = crypto::Sm2AccessPoint::GetInstance()->Create();
    EXPECT_TRUE(key);
  }
};

namespace crypto {

TEST(Sm2AccessPoint, Basic) {
  EXPECT_TRUE(Sm2AccessPoint::GetInstance());
  std::unique_ptr<AsymmetricKey> key = Sm2AccessPoint::GetInstance()->Create(Sm2AccessPoint::kKey256Bits);
  EXPECT_TRUE(key);

  std::vector<uint8_t> public_key;
  std::vector<uint8_t> private_key;
  EXPECT_TRUE(key->ExportPublicKey(&public_key));
  EXPECT_TRUE(key->ExportPrivateKey(&private_key));
  std::string cipher;
  EXPECT_TRUE(Sm2AccessPoint::GetInstance()->PublickKeyEncrypt(public_key, kMediaKeyPath, &cipher));
  LOG(INFO) << "public_key: " << base::HexEncode((const char *)public_key.data(), public_key.size());
  LOG(INFO) << "private_key: " << base::HexEncode((const char *)private_key.data(), private_key.size());
  LOG(INFO) << "cipher: " << base::HexEncode(cipher);

#if 0
  std::string file_content;
  EXPECT_TRUE(base::ReadFileToString(base::FilePath(kM3u8Path), &file_content));
  //LOG(INFO) << "file_content: " << file_content;
  Replace(file_content, kVideoKeyPathAnchor, base::HexEncode(cipher));
  LOG(INFO) << "file_content: " << file_content;
  EXPECT_TRUE(base::WriteFile(base::FilePath(kEncM3u8Path), file_content.data(), file_content.size()));
#endif
}

TEST(Sm2AccessPointTest, MultiThread) {
  std::vector<std::thread> thread_pool;
  for (int i = 0; i < kMaxWorkers; ++i) {
    thread_pool.push_back(std::thread(GenerateKeyPairWorker()));
  }
  for (int i = 0; i < kMaxWorkers; ++i) {
    thread_pool[i].join();
  }
}

} // namespace crypto
