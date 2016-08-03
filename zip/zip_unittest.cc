#include "zip/zip.h"
#include "zip/zip_reader.h"

#include <stddef.h>
#include <stdint.h>

#include <set>
#include <string>
#include <vector>

#include "base/file.h"
#include "base/file_path.h"
#include "base/file_enumerator.h"
#include "base/file_util.h"
#include "base/scoped_temp_dir.h"

#include <gtest/gtest.h>

using namespace zip;

TEST(UnzipFile, Basic) {
  base::FilePath path("/tmp/1.epub");
  LOG(INFO) << path.value();
  ASSERT_TRUE(base::PathExists(path)) << "no file " << path.value();
  base::FilePath test_dir("/tmp/test_epub/");
  ASSERT_TRUE(zip::Unzip(path, test_dir));

  base::FileEnumerator files(test_dir, true,
                             base::FileEnumerator::FILES |
                             base::FileEnumerator::DIRECTORIES);
  base::FilePath next_path = files.Next();
  while (!next_path.value().empty()) {
    LOG(INFO) << "Extracting " << next_path.value();
    next_path = files.Next();
  }

  base::FilePath src_dir = test_dir;
  base::FilePath new_dir("/tmp/a.epub");
  
  EXPECT_TRUE(zip::Zip(src_dir, new_dir, true));
}
