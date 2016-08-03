// Usgae:
//   string result = StringPrintf("%d %s\n", 27, "Hello");
//   SStringPrintf(&result, "%d %s\n", 28, "Hello");
//   StringAppendF(&result, "%d %s\n", 20, "there");
//
//
#ifndef BASE_STRING_PRINTF_H_
#define BASE_STRING_PRINTF_H_
#include <stdarg.h>
#include <string>
#include <vector>

namespace base {

extern std::string StringPrintf(const char* format, ...);
extern const std::string& SStringPrintf(std::string* dst, const char* format, ...);
extern void StringAppendF(std::string* dst, const char* format, ...);
extern void StringAppendV(std::string* dst, const char* format, va_list ap);

extern const int kStringPrintfVectorMaxArgs;

extern std::string StringPrintfVector(const char* format, 
                                      const std::vector<std::string>& v);
} // namespace base
#endif // BASE_STRING_PRINTF_H_
