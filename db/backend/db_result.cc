#include "db/backend/db_result.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

namespace db {

bool DBResult::ParseTimeString(const char* str, 
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

  timeval val;
  val.tv_sec = ::mktime(&tm);
  val.tv_usec = 0; // TODO(wqx)???

  //
  // TODO(wqx)?
  // base::Time::FromTimeT has BUG!
  // when use base::Time::FromTimeT(::mkdtime(&tm));
  // you Should always use base::Time::FromTimeval instead!
  //
  *t = base::Time::FromTimeval(val);

  return true;
}

} // namespace
