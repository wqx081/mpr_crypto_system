#include "crypto/asymmetric_padding.h"
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
  EXPECT_EQ(s, "10\00\00\00\00\00\00");
}

} // namespace crypto
