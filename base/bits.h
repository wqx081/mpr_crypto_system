#ifndef BASE_BITS_H_
#define BASE_BITS_H_

#include <stddef.h>
#include <stdint.h>

#include <glog/logging.h>

namespace base {
namespace bits {

inline int Log2Floor(uint32_t n) {
  if (n == 0)
    return -1;
  int log = 0;
  uint32_t value = n;
  for (int i = 4; i >= 0; --i) {
    int shift = (1 << i);
    uint32_t x = value >> shift;
    if (x != 0) {
      value = x;
      log += shift;
    }
  }
  DCHECK_EQ(value, 1u);
  return log;
}

inline int Log2Ceiling(uint32_t n) {
  if (n == 0) {
    return -1;
  } else {
    return 1 + Log2Floor(n - 1);
  }
}

inline size_t Align(size_t size, size_t alignment) {
  DCHECK_EQ(alignment & (alignment - 1), 0u);
  return (size + alignment - 1) & ~(alignment - 1);
}


} // namespace bits
} // namespace base
#endif // BASE_BITS_H_
