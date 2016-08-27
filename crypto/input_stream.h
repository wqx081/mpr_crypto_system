//@Author: WangQixiang (wangqx at mpreader.com)
//@Date: 2016/08/27
//
#ifndef CRYPTO_INPUT_STREAM_H_
#define CRYPTO_INPUT_STREAM_H_
#include "crypto/type.h"

namespace crypto {

class InputStream {
 public:
  virtual ~InputStream() {}

  virtual size_t Available() = 0;
  virtual void Close() = 0;
  virtual Byte Read() = 0;
  virtual size_t Read(ByteVector* b) = 0;
  virtual size_t Read(ByteVector* b, size_t offset, size_t length) = 0;
  virtual void Reset() = 0;
  virtual int64_t Skip(int64_t n) = 0;
};

} // namespace crypto
#endif // CRYPTO_INPUT_STREAM_H_
