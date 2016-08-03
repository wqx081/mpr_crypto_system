#include "base/string_encode.h"

#include <stdio.h>
#include <stdlib.h>

namespace base {


size_t UTF8Encode(char* buffer, size_t buf_len, unsigned long value) {
  if ((value <= 0x7F) && (buf_len >= 1)) {
    buffer[0] = static_cast<unsigned char>(value);
    return 1;
  }
  if ((value <= 0x7FF) && (buf_len >= 2)) {
    buffer[0] = 0xC0 | static_cast<unsigned char>(value >> 6);
    buffer[1] = 0x80 | static_cast<unsigned char>(value & 0x3F);
    return 2;
  }
  if ((value <= 0xFFFF) && (buf_len >= 3)) {
    buffer[0] = 0xE0 | static_cast<unsigned char>(value >> 12);
    buffer[1] = 0x80 | static_cast<unsigned char>((value >> 6) & 0x3F);
    buffer[2] = 0x80 | static_cast<unsigned char>(value & 0x3F);
    return 3;
  }
  if ((value <= 0x1FFFFF) && (buf_len >= 4)) {
    buffer[0] = 0xF0 | static_cast<unsigned char>(value >> 18);
    buffer[1] = 0x80 | static_cast<unsigned char>((value >> 12) & 0x3F);
    buffer[2] = 0x80 | static_cast<unsigned char>((value >> 6) & 0x3F);
    buffer[3] = 0x80 | static_cast<unsigned char>(value & 0x3F);
    return 4;
  }
  return 0;
}

size_t UTF8Decode(const char* source, size_t src_len, unsigned long* value) {
  const unsigned char* s = reinterpret_cast<const unsigned char*>(source);
  if ((s[0] & 0x80) == 0x00) {                    // Check s[0] == 0xxxxxxx
    *value = s[0];
    return 1;
  }
  if ((src_len < 2) || ((s[1] & 0xC0) != 0x80)) {  // Check s[1] != 10xxxxxx
    return 0;
  }
  unsigned long value16 = (s[1] & 0x3F);
  if ((s[0] & 0xE0) == 0xC0) {                    // Check s[0] == 110xxxxx
    *value = ((s[0] & 0x1F) << 6) | value16;      
    return 2;
  } 
  if ((src_len < 3) || ((s[2] & 0xC0) != 0x80)) {  // Check s[2] != 10xxxxxx
    return 0; 
  } 
  value16 = (value16 << 6) | (s[2] & 0x3F);
  if ((s[0] & 0xF0) == 0xE0) {                    // Check s[0] == 1110xxxx
    *value = ((s[0] & 0x0F) << 12) | value16;     
    return 3;
  } 
  if ((src_len < 4) || ((s[3] & 0xC0) != 0x80)) {  // Check s[3] != 10xxxxxx
    return 0; 
  } 
  value16 = (value16 << 6) | (s[3] & 0x3F);
  if ((s[0] & 0xF8) == 0xF0) {                    // Check s[0] == 11110xxx
    *value = ((s[0] & 0x07) << 18) | value16;     
    return 4;
  } 
  return 0;
}

// Hex Encode/Decode
static const char kHex[] = "0123456789ABCDEF";

char HexEncode(unsigned char v) {
  DCHECK_LT(v, 16);
  return (v < 16) ? kHex[v] : '!';
}

bool HexDecode(char ch, unsigned char* v) {
  if ((ch >= '0') && (ch <= '9')) {
    *v = ch - '0';
  } else if ((ch >= 'A') && (ch <= 'Z')) {
    *v = ch - 'A' + 10;
  } else if ((ch >= 'a') && (ch <= 'z')) {
    *v = ch - 'a' + 10;
  } else {
    return false;
  }
  return true;
}

size_t HexEncode(char* buf, size_t buf_len,
                 const char* source, size_t src_len) {
  return HexEncodeWithDelimiter(buf, buf_len, source, src_len, 0);
}

size_t HexEncodeWithDelimiter(char* buf, size_t buf_len,
                              const char* src, size_t src_len,
                              char delimiter) {
  DCHECK(buf);
  if (buf_len == 0) {
    return 0;
  }

  const unsigned char* src_bounds = reinterpret_cast<const unsigned char *>(src);
  size_t src_pos = 0;
  size_t buf_pos = 0;
  size_t needed = delimiter ? (src_len * 3) : (src_len * 2 + 1);
  if (buf_len < needed) {
    return 0;
  }

  while (src_pos < src_len) {
    unsigned char ch = src_bounds[src_pos++];
    buf[buf_pos]   = HexEncode((ch >> 4) & 0xF);
    buf[buf_pos+1] = HexEncode((ch     ) & 0xF);
    buf_pos += 2;

    if (delimiter && (src_pos < src_len)) {
      buf[buf_pos] = delimiter;
      ++buf_pos;
    }
  }

  buf[buf_pos] = '\0';
  return buf_pos;
}

std::string HexEncode(const std::string& str) {
  return HexEncode(str.c_str(), str.size());
}

std::string HexEncode(const char* src, size_t src_len) {
  return HexEncodeWithDelimiter(src, src_len, 0);
}

std::string HexEncodeWithDelimiter(const char* src, size_t src_len, char delimiter) {

  const size_t kBufferSize = src_len * 3;
  char buf[kBufferSize];
  size_t length = HexEncodeWithDelimiter(buf, kBufferSize, src, src_len, delimiter);

  DCHECK(src_len == 0 || length > 0);
  return std::string(buf, length);
}

///////
size_t HexDecode(char* buf, size_t buf_len,
                 const char* src, size_t src_len) {
  return HexDecodeWithDelimiter(buf, buf_len, src, src_len, 0);
}

size_t HexDecodeWithDelimiter(char* buf, size_t buf_len,
                              const char* src, size_t src_len,
                              char delimiter) {
  DCHECK(buf);
  if (buf_len == 0) {
    return 0;
  }

  unsigned char* buf_bound = reinterpret_cast<unsigned char *>(buf);
  size_t src_pos = 0;
  size_t buf_pos = 0;
  size_t needed = (delimiter) ? (src_len + 1) / 3 : src_len / 2;
  if (buf_len < needed) {
    return 0;
  }

  while (src_pos < src_len) {
    if ((src_len - src_pos) < 2) {
      return 0;
    }

    unsigned char h1, h2;
    if (!HexDecode(src[src_pos], &h1) ||
        !HexDecode(src[src_pos + 1], &h2)) {
      return 0;
    }

    buf_bound[buf_pos++] = (h1 << 4) | h2;
    src_pos += 2;

    if (delimiter && (src_len - src_pos) > 1) {
      if (src[src_pos] != delimiter) {
        return 0;
      }
      ++src_pos;
    }
  }
  return buf_pos;
}

size_t HexDecode(char* buf, size_t buf_len, const std::string& src) {
  return HexDecodeWithDelimiter(buf, buf_len, src, 0);
}

size_t HexDecodeWithDelimiter(char* buf, size_t buf_len,
                              const std::string& src, char delimiter) {
  return HexDecodeWithDelimiter(buf, buf_len,
                                src.c_str(), src.length(), delimiter);
}

std::string HexDecode(const std::string& src) {
  if (src.size() == 0) {
    return "";
  }
  const size_t kBufferSize = src.size() / 2; // / 2;
  char buf[kBufferSize];
  size_t length = HexDecode(buf, kBufferSize, src);
  DCHECK(length > 0);
  return std::string(buf, length); 
}

} // namespace base
