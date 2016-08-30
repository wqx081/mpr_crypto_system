#include "crypto/file_input_stream.h"
#include "crypto/file_output_stream.h"

#include <glog/logging.h>
#include <gtest/gtest.h>

const char* kNotExistedFile = "/dead/file.h";
const char* kExistedFile = "/tmp/test.cc";
const char* kTargetFile = "/tmp/test_1.cc";

namespace crypto {

TEST(FileInputStream, Basic) {
  FileInputStream in;
  FileOutputStream out;

  EXPECT_FALSE(in.Open(kNotExistedFile));
  EXPECT_TRUE(in.Open(kExistedFile));
  EXPECT_TRUE(out.Open(kTargetFile));

  while (in.Available()) {
    ByteVector buffer;
    buffer.resize(16);
    in.Read(&buffer);
    LOG(INFO) << "Buffer: " << std::string((char *)buffer.data(), buffer.size());
    EXPECT_TRUE(out.Write(&buffer));
  }
}

} // namespace crypto
