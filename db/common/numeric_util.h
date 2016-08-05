#ifndef DB_COMMON_NUMERIC_UTIL_H_
#define DB_COMMON_NUMERIC_UTIL_H_

#include "db/common/exception.h"

#include <string>
#include <sstream>
#include <limits>
#include <iomanip>


namespace db {

template<typename T>
T ParseNumber(const std::string& s, std::istringstream& ss) {
  ss.clear();
  ss.str(s);
  if (s.find_first_of(".eEdD") != std::string::npos) {
    long double v;
    ss >> v;
    if (ss.fail() || !std::ws(ss).eof()) {
      throw BadValueCast();
    }

    if (std::numeric_limits<T>::is_integer) {
      if (v > std::numeric_limits<T>::max() ||
          v < std::numeric_limits<T>::min()) {
        throw BadValueCast();
      }
    }

    return static_cast<T>(v);
  }
  T v;
  ss >> v;
  if (ss.fail() || !std::ws(ss).eof()) {
    throw BadValueCast();
  }
  if (std::numeric_limits<T>::is_integer &&
      !std::numeric_limits<T>::is_signed &&
      s.find('-') != std::string::npos   &&
      v != 0) {
    throw BadValueCast();
  }

  return v;
}

} // namespace db
#endif // DB_COMMON_NUMERIC_UTIL_H_
