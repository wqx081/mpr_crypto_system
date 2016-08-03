// This header defines cross-platform ByteSwap() implementations for 16, 32 and
// 64-bit values, and NetToHostXX() / HostToNextXX() functions equivalent to
// the traditional ntohX() and htonX() functions.
// Use the functions defined here rather than using the platform-specific
// functions directly.

#ifndef BASE_SYS_BYTEORDER_H_
#define BASE_SYS_BYTEORDER_H_

#include <arpa/inet.h>

// Include headers to provide byteswap for all platforms.
#include <byteswap.h>


namespace base {

// Returns a value with all bytes in |x| swapped, i.e. reverses the endianness.
inline uint16_t ByteSwap(uint16_t x) {
  return bswap_16(x);
}
inline uint32_t ByteSwap(uint32_t x) {
  return bswap_32(x);
}
inline uint64_t ByteSwap(uint64_t x) {
  return bswap_64(x);
}

// Converts the bytes in |x| from host order (endianness) to little endian, and
// returns the result.
inline uint16_t ByteSwapToLE16(uint16_t x) {
  return x;
}
inline uint32_t ByteSwapToLE32(uint32_t x) {
  return x;
}
inline uint64_t ByteSwapToLE64(uint64_t x) {
  return x;
}

// Converts the bytes in |x| from network to host order (endianness), and
// returns the result.
inline uint16_t NetToHost16(uint16_t x) {
  return ByteSwap(x);
}
inline uint32_t NetToHost32(uint32_t x) {
  return ByteSwap(x);
}
inline uint64_t NetToHost64(uint64_t x) {
  return ByteSwap(x);
}

// Converts the bytes in |x| from host to network order (endianness), and
// returns the result.
inline uint16_t HostToNet16(uint16_t x) {
  return ByteSwap(x);
}
inline uint32_t HostToNet32(uint32_t x) {
  return ByteSwap(x);
}
inline uint64_t HostToNet64(uint64_t x) {
  return ByteSwap(x);
}

}  // namespace base


#endif  // BASE_SYS_BYTEORDER_H_
