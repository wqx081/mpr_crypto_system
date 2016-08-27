//@Author: WangQiXiang (wangqx at mpreader.com)
//@Date: 2016/08/27
//
#ifndef CRYPTO_HTYPE_H_
#define CRYPTO_HTYPE_H_
#include <stdint.h>
#include <stddef.h>
#include <vector>

namespace crypto {

typedef uint8_t  Byte;
typedef uint16_t Word;
typedef uint32_t DWord;
typedef uint64_t QWord;

typedef std::vector<Byte> ByteVector;

} // namespace crypto
#endif // CRYPTO_HTYPE_H_
