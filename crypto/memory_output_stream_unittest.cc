#include "crypto/memory_output_stream.h"
#include "crypto/type.h"

#include <algorithm>
#include <gtest/gtest.h>

namespace {

const char* kTestData =
  "01234567890123456789012345678901234567890123456789"  // 50
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwx"  // 100
  "yz";                                                 // 102
const size_t kTestBufferLen = 102;

} // namespace

namespace crypto {

TEST(MemoryOutputStream, Basic) {

  ByteVector test_buffer;
  test_buffer.resize(kTestBufferLen);
  std::copy(kTestData, kTestData + kTestBufferLen, test_buffer.begin());
  
  MemoryOutputStream os;
  os.Write(&(test_buffer[0]), (int32_t)50, (int32_t)(kTestBufferLen - 50));
  EXPECT_EQ(os.size(), kTestBufferLen - 50);
  EXPECT_EQ(memcmp(os.data(), &(test_buffer[0]) + 50, kTestBufferLen - 50), 0);
}

} // namespace crypto
