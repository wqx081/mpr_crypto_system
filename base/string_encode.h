#ifndef BASE_STRING_ENCODE_H_
#define BASE_STRING_ENCODE_H_
#include <sstream>
#include <string>
#include <vector>

#include <glog/logging.h>

namespace base {

size_t UTF8Encode(char* buf, size_t len, unsigned long value);
size_t UTF8Decode(const char* source, size_t source_len, unsigned long* value);

size_t Escape(char* buf, size_t buf_len,
              const char* source, size_t source_len,
              const char* illegal, char escape);
size_t Unescape(char* buffer, size_t buf_len,
                const char* source, size_t source_len,
                char escape);


// Hex Encode/Decode
char HexEncode(unsigned char val);
bool HexDecode(char ch, unsigned char* val);
size_t HexEncode(char* buf, size_t buf_len, const char* source, size_t source_len);
size_t HexEncodeWithDelimiter(char* buf, size_t buf_len,
                              const char* source, size_t source_len,
                              char delimiter);
std::string HexEncode(const std::string& str);
std::string HexEncode(const char* source, size_t source_len);
std::string HexEncodeWithDelimiter(const char* source, size_t source_len, char delimiter);

size_t HexDecode(const char* buf, size_t buf_len, const char* src, size_t src_len);
size_t HexDecodeWithDelimiter(char* buf, size_t buf_len, 
                              const char* src, size_t src_len, 
                              char delimiter);
size_t HexDecode(char* buf, size_t buf_len, const std::string& source);
size_t HexDecodeWithDelimiter(char* buf, size_t buf_len, const std::string& source, char delimiter);
std::string HexDecode(const std::string& src);

// 
template<typename T>
static bool ToString(const T& t, std::string* s) {
  DCHECK(s);
  std::ostringstream oss;
  oss << std::boolalpha << t;
  *s = oss.str();
  return !oss.fail();
}

template<typename T>
static bool FromString(const std::string& s, T* t) {
  DCHECK(t);
  std::istringstream iss(s);
  iss >> std::boolalpha >> *t;
  return !iss.fail();
}

template<typename T>
static inline std::string ToString(const T& val) {
  std::string str;
  ToString(val, &str);
  return str;
}

template<typename T>
static inline T FromString(const std::string& str) {
  T val;
  FromString(str, &val);
  return val;
}

template<typename T>
static inline T FromString(const T& default_value, const std::string& str) {
  T val(default_value);
  FromString(str, &val);
  return val;
}


} // namespace
#endif // BASE_STRING_ENCODE_H_
