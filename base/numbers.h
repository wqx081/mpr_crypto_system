#ifndef BASE_NUMBERS_H_
#define BASE_NUMBERS_H_

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <functional>
using std::binary_function;
using std::less;
#include <limits>
#include <string>
#include <type_traits>

#include "base/string_piece.h"

namespace base {

int32_t strto32_adapter(const char *nptr, char **endptr, int base);
uint32_t strtou32_adapter(const char *nptr, char **endptr, int base);

inline int32_t strto32(const char *nptr, char **endptr, int base) {
  if (sizeof(int32_t) == sizeof(long))
    return static_cast<int32_t>(strtol(nptr, endptr, base));
  else
    return strto32_adapter(nptr, endptr, base);
}

inline uint32_t strtou32(const char *nptr, char **endptr, int base) {
  if (sizeof(uint32_t) == sizeof(unsigned long))
    return static_cast<uint32_t>(strtoul(nptr, endptr, base));
  else
    return strtou32_adapter(nptr, endptr, base);
}

inline int64_t strto64(const char *nptr, char **endptr, int base) {
  static_assert(sizeof(int64_t) == sizeof(long long),
                "sizeof_int64_is_not_sizeof_long_long");
  return strtoll(nptr, endptr, base);
}
  
inline uint64_t strtou64(const char *nptr, char **endptr, int base) {
  static_assert(sizeof(uint64_t) == sizeof(unsigned long long),
                 "sizeof_uint64_is_not_sizeof_long_long");
  return strtoull(nptr, endptr, base);
}

inline int32_t atoi32(const char *nptr) {
  return strto32(nptr, NULL, 10);
}
  
inline int64_t atoi64(const char *nptr) {
  return strto64(nptr, NULL, 10);
}

inline int32_t atoi32(const std::string &s) {
  return atoi32(s.c_str());
}
  
inline int64_t atoi64(const std::string &s) {
  return atoi64(s.c_str());
}

////////////
bool safe_strto32_base(StringPiece text, int32_t* value, int base);
bool safe_strto64_base(StringPiece text, int64_t* value, int base);
bool safe_strtou32_base(StringPiece text, uint32_t* value, int base);
bool safe_strtou64_base(StringPiece text, uint64_t* value, int base);
bool safe_strtosize_t_base(StringPiece text, size_t* value, int base);

inline bool safe_strto32(StringPiece text, int32_t* value) {
  return safe_strto32_base(text, value, 10);
}
  
inline bool safe_strto64(StringPiece text, int64_t* value) {
  return safe_strto64_base(text, value, 10);
}
  
inline bool safe_strtou32(StringPiece text, uint32_t* value) {
  return safe_strtou32_base(text, value, 10);
}
  
inline bool safe_strtou64(StringPiece text, uint64_t* value) {
  return safe_strtou64_base(text, value, 10);
}
  
inline bool safe_strtosize_t(StringPiece text, size_t* value) {
  return safe_strtosize_t_base(text, value, 10);
}

inline bool safe_strto32(const char* str, int32_t* value) {
  return safe_strto32(StringPiece(str), value);
}
  
inline bool safe_strto32(const std::string& str, int32_t* value) {
  return safe_strto32(StringPiece(str), value);
}
  
  
inline bool safe_strto64(const char* str, int64_t* value) {
  return safe_strto64(StringPiece(str), value);
}
  
inline bool safe_strto64(const std::string& str, int64_t* value) {
  return safe_strto64(StringPiece(str), value);
}
  
inline bool safe_strtou32(const char* str, uint32_t* value) {
  return safe_strtou32(StringPiece(str), value);
}
  
inline bool safe_strtou32(const std::string& str, uint32_t* value) {
  return safe_strtou32(StringPiece(str), value);
}
  
inline bool safe_strtou64(const char* str, uint64_t* value) {
  return safe_strtou64(StringPiece(str), value);
} 
  
inline bool safe_strtou64(const std::string& str, uint64_t* value) {
  return safe_strtou64(StringPiece(str), value);
} 

// Float
bool safe_strtof(const char* str, float* value);
bool safe_strtof(const std::string& str, float* value);
bool safe_strtod(const char* str, double* value);
bool safe_strtod(const std::string& str, double* value);

struct DoubleRangeOptions {
  const char* separators;
  bool require_separator;
  const char* acceptable_terminators;
  bool null_terminator_ok;
  bool allow_unbounded_markers;
  //uint32_t num_required_bounds;
  int32_t num_required_bounds;
  bool dont_modify_unbounded;
  bool allow_currency;
  bool allow_comparators;
};

bool ParseDoubleRange(const char* text, int len, const char** end,
                      double* from, double* to, bool* is_currency,
                      const DoubleRangeOptions& opts);

// 
template<typename T>
inline bool StringAsValue(const std::string& str, T* value) {
  bool ret = false;

  if (std::is_signed<T>::value) {
    if (std::numeric_limits<T>::max() <= std::numeric_limits<int32_t>::max()) {
      int32_t v;
      ret = safe_strto32(str, &v);
      *value = static_cast<T>(v);
    } else {
      int64_t v;
      ret = safe_strto64(str, &v);
      *value = static_cast<T>(v);
    }    
  } else {
    if (std::numeric_limits<T>::max() <= std::numeric_limits<uint32_t>::max()) {
      uint32_t v;
      ret = safe_strtou32(str, &v);
      *value = static_cast<T>(v);
    } else {
      uint64_t v;
      ret = safe_strtou64(str, &v);
      *value = static_cast<T>(v);
    }
  }
  return ret;
}

template<>
inline bool StringAsValue(const std::string& str, float* value) {
  return safe_strtof(str, value);
}

template<>
inline bool StringAsValue(const std::string& str, double* value) {
  return safe_strtod(str, value);
}

//TODO...
} // namespace base
#endif // BASE_NUMBERS_H_
