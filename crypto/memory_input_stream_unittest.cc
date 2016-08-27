#include "crypto/memory_input_stream.h"
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

TEST(MemoryInputStream, Basic) {
  ByteVector buffer;
  buffer.resize(kTestBufferLen);
  std::copy(kTestData, kTestData + kTestBufferLen, buffer.begin());

  MemoryInputStream is(&(buffer[0]), kTestBufferLen);
  EXPECT_EQ(is.Available(), kTestBufferLen); 

  // Read one Byte
  EXPECT_EQ(is.Read(), '0');
  EXPECT_EQ(is.Read(), '1');
  EXPECT_EQ(is.Read(), '2');

  // Read byte vector
  ByteVector b;
  b.resize(7);
  EXPECT_EQ(is.Read(&b), 7); // position 10
  EXPECT_EQ(memcmp(&(b[0]), &(buffer[0]) + 3, 7), 0);

  b.resize(17);
  EXPECT_EQ(is.Read(&b, 7, 10), 10); // position 20
  EXPECT_EQ(memcmp(&(b[0]), &(buffer[0]) + 3, 17), 0);

  // Skip over for me! ^_^
  b.clear();
  b.resize(10);
  EXPECT_EQ(is.Skip(30), 30);   // position 50
  EXPECT_EQ(is.Read(&b), 10);   // position 60
  EXPECT_EQ(memcmp(&(b[0]), &(buffer[0]) + 50, 10), 0);
  b.clear();
  b.resize(10);
  EXPECT_EQ(is.Skip(-20), -20);  // position 40
  EXPECT_EQ(is.Read(&b), 10);  // position 50
  EXPECT_EQ(memcmp(&(b[0]), &(buffer[0]) + 40, 10), 0);
  
  EXPECT_EQ(is.Available(), (int32_t)kTestBufferLen - 50);
  EXPECT_EQ(is.Skip(-60), -50);  // Out of bound, position 0
  EXPECT_EQ(is.Skip(kTestBufferLen + 10), (int32_t)kTestBufferLen);
}

} // namespace crypto
