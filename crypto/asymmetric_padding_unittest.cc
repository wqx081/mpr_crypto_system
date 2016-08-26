#include "crypto/asymmetric_padding.h"

#include <glog/logging.h>
#include <gtest/gtest.h>

static const int kBlockSize = 8;

namespace crypto {

TEST(ZeroPaddingTest, FillPadding) {
  ZeroPadding zero_padding;
  std::string s(""); 
  zero_padding.FillPadding(s, kBlockSize);
  EXPECT_EQ(s, "");
  s = "12345678";
  zero_padding.FillPadding(s, kBlockSize);
  EXPECT_EQ(s, "12345678");
  s = "10";
  zero_padding.FillPadding(s, kBlockSize); 
  zero_padding.TrimPadding(s, kBlockSize);
  EXPECT_EQ(s, "10");
}

TEST(MprPaddingTest, Test) {
  MprPadding mpr_padding;
  std::string s("");
  mpr_padding.FillPadding(s, kBlockSize);
  EXPECT_EQ(s, "\x08\x08\x08\x08\x08\x08\x08\x08");
  s = "1";
  mpr_padding.FillPadding(s, kBlockSize);
  mpr_padding.TrimPadding(s, kBlockSize);
  EXPECT_EQ(s, "1");
  s = "12345678";
  mpr_padding.FillPadding(s, kBlockSize);
  LOG(INFO) << "padding: " << s.size();
}

} // namespace crypto
