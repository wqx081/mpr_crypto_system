#include "base/time.h"
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <nspr/prtime.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

namespace base {


bool ParseTimeString(const char* str,
                     base::Time* t,
                     const std::string& format) {
  if (str == nullptr || t == nullptr) {
    return false;
  }
  if (str[0] == '\0') {
    return false;
  }

  struct tm tm;
  memset(&tm, 0, sizeof(tm));
  if (::strptime(str, format.c_str(), &tm) == nullptr) {
    return false;
  }
  time_t orig = ::mktime(&tm);
  timeval val;
  val.tv_sec = orig;
  val.tv_usec = 0;

  *t = base::Time::FromTimeval(val);
  LOG(INFO) << "----time---------: " << (*t).Format() << " <--- time0: " << str;
  return true;
}


TEST(Time, FromString) {
  base::Time t;

  EXPECT_TRUE(ParseTimeString("2016-08-10 15:47:04", &t, "%Y-%m-%d %H:%M:%S"));
}

}
