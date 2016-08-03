#include "base/string_printf.h"
#include "base/macros.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <vector>

namespace base {

void StringAppendV(std::string* dst, const char* format, va_list ap) {
  static const int kSpaceLength = 1024;
  char space[kSpaceLength];

  va_list ap_copy;
  va_copy(ap_copy, ap);
  int result = vsnprintf(space, kSpaceLength, format, ap_copy);
  va_end(ap_copy);

  if (result < kSpaceLength) {
    if (result >= 0) {
      dst->append(space, result);
      return;
    }
    if (result < 0) {
      return;
    }
  }

  int length = result + 1;
  char* buf = new char[length];

  va_copy(ap_copy, ap);
  result = vsnprintf(buf, length, format, ap_copy);
  va_end(ap_copy);

  if (result >= 0 && result < length) {
    dst->append(buf, result);
  }
  delete[] buf;
}

std::string StringPrintf(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  std::string result;
  StringAppendV(&result, format, ap);
  va_end(ap);
  return result;
}

const std::string& SStringPrintf(std::string* dst, const char* format, ...) {
  va_list ap;
  va_start(ap, format),
  dst->clear();
  StringAppendV(dst, format, ap);
  va_end(ap);
  return *dst;
}

void StringAppendF(std::string* dst, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  StringAppendV(dst, format, ap);
  va_end(ap);
}

const int kStringPrintfVectorMaxArgs = 32;
static const char string_printf_empty_block[256] = {'\0'};

std::string StringPrintfVector(const char* format,
                               const std::vector<std::string>& v) {
  const char* cstr[kStringPrintfVectorMaxArgs];
  for (size_t i = 0; i < v.size(); ++i) {
    cstr[i] = v[i].c_str();
  }
  for (size_t i = v.size(); i < ARRAYSIZE(cstr); ++i) {
    cstr[i] = &string_printf_empty_block[0];
  }
  return StringPrintf(format,
                      cstr[0], cstr[1], cstr[2], cstr[3], cstr[4],
                      cstr[5], cstr[6], cstr[7], cstr[8], cstr[9],
                      cstr[10], cstr[11], cstr[12], cstr[13], cstr[14],
                      cstr[15], cstr[16], cstr[17], cstr[18], cstr[19],
                      cstr[20], cstr[21], cstr[22], cstr[23], cstr[24],
                      cstr[25], cstr[26], cstr[27], cstr[28], cstr[29],
                      cstr[30], cstr[31]);
}


} // namespace base
