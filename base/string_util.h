#ifndef BASE_STRING_UTIL_H_
#define BASE_STRING_UTIL_H_

#include <ctype.h>
#include <stdarg.h>   // va_list
#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "base/macros.h"
#include "base/string_piece.h"  // For implicit conversions.

namespace base {

inline char ToLowerASCII(char c) {
  return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
}

template<typename Char> struct CaseInsensitiveCompareASCII {
 public:
  bool operator()(Char x, Char y) const {
    return ToLowerASCII(x) == ToLowerASCII(y);
  }
};

bool LowerCaseEqualsASCII(StringPiece str, StringPiece lowercase_ascii);

enum class CompareCase {
  SENSITIVE,
  INSENSITIVE_ASCII,
};

bool StartsWith(StringPiece str,
		StringPiece search_for,
		CompareCase case_sensitivity);

bool EndsWith(StringPiece str,
              StringPiece search_for,
	      CompareCase case_sensitivity);

char* WriteInto(std::string* str, size_t length_with_null);

// Trim
enum TrimPositions {
  TRIM_NONE    = 0,
  TRIM_LEADING = 1,
  TRIM_TRAILING= 2,
  TRIM_ALL     = TRIM_LEADING | TRIM_TRAILING,
};

bool TrimString(const std::string& input,
                StringPiece trim_chars,
                std::string* output);

StringPiece TrimString(StringPiece input,
                       const StringPiece& trim_chars,
                       TrimPositions positions);

} // namespace
#endif // BASE_STRING_UTIL_H_
