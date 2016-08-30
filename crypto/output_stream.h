//@Author: WangQixiang (wangqx at mpreader.com)
//@Date: 2016/08/27
//
#ifndef CRYPTO_OUTPUT_STREAM_H_
#define CRYPTO_OUTPUT_STREAM_H_
#include "crypto/type.h"
#include <string>

namespace crypto {

class OutputStream {
 public:
  virtual ~OutputStream() {}

  virtual void Close() = 0;
  virtual bool Flush() = 0;
  virtual int Write(ByteVector* buffer) = 0;
  virtual int Write(Byte n) = 0;
  virtual int Write(ByteVector* buffer, size_t offset, size_t length) = 0;
  virtual int Write(Byte* buffer, size_t offset, size_t length) = 0;
  virtual std::string ToString() = 0;
};

} // namespace crypto
#endif // CRYPTO_OUTPUT_STREAM_H_
