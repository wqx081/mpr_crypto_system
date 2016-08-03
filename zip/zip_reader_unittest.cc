#include "zip/zip_reader.h"
#include "base/file_path.h"

#include <gtest/gtest.h>

using namespace zip;

namespace base {

TEST(ZipReader, Basic) {
  FilePath zip_path("./1.epub");

  ZipReader zip_reader;
  EXPECT_TRUE(zip_reader.Open(zip_path));
}

} // namespace base
