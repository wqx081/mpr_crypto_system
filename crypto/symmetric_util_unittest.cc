#include "crypto/symmetric_util.h"
#include "crypto/symmetric_key.h"

#include "crypto/file_input_stream.h"
#include "crypto/file_output_stream.h"

#include <glog/logging.h>
#include <gtest/gtest.h>

const char* kSourceFile= "/tmp/test.cc";
const char* kEncFile = "/tmp/test_enc.cc";
const char* kDecFile = "/tmp/test_dec.cc";

namespace crypto {

TEST(SymmtricUtil, Basic) {
  FileInputStream in;
  FileOutputStream out;

  EXPECT_TRUE(in.Open(kSourceFile));
  EXPECT_TRUE(out.Open(kEncFile));

  std::unique_ptr<SymmetricKey> key = SymmetricKey::GenerateRandomKey(
    SymmetricKey::AES,
    128);
  DCHECK(key);

  std::string raw_key;
  EXPECT_TRUE(key->GetRawKey(&raw_key));
  EXPECT_TRUE(EcbEncrypt(raw_key, &in, &out));

  in.Close();
  out.Close();

  EXPECT_TRUE(in.Open(kEncFile));
  EXPECT_TRUE(out.Open(kDecFile));
  EXPECT_TRUE(EcbDecrypt(raw_key, &in, &out));

  const std::string iv("aaaabbbbccccdddd");
  in.Close();
  out.Close();
  EXPECT_TRUE(in.Open(kSourceFile));
  EXPECT_TRUE(out.Open(kEncFile));
  EXPECT_TRUE(CbcEncrypt(raw_key, iv, &in, &out));
  in.Close();
  out.Close();
  EXPECT_TRUE(in.Open(kEncFile));
  EXPECT_TRUE(out.Open(kDecFile));
  EXPECT_TRUE(CbcDecrypt(raw_key, iv, &in, &out));
}

} // namespace crypto
