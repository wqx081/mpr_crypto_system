#include "base/numbers.h"
#include <gtest/gtest.h>

namespace base {

TEST(TO64, Basic) {
  int64_t value;
  EXPECT_TRUE(StringAsValue<int64_t>("123444556", &value));
}

}
