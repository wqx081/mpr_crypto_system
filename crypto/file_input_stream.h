//@Author: WangQixiang (wangqx at mpreader.com)
//@Date: 2016/08/20
//
#ifndef CRYPTO_FILE_INPUT_STREAM_H_
#define CRYPTO_FILE_INPUT_STREAM_H_
#include <stdio.h>

#include "crypto/input_stream.h"

namespace crypto {
  
class FileInputStream : public InputStream {
 public:
  FileInputStream();
  virtual ~FileInputStream() override;

  // From Inputstream
  virtual size_t Available() override;
  virtual void Close() override;
  virtual Byte Read() override;
  virtual size_t Read(ByteVector* b) override;
  virtual size_t Read(ByteVector* b, size_t offset, size_t length) override;
  virtual void Reset() override;
  virtual int64_t Skip(int64_t n) override;

  bool Open(const char*  file_name);

 private:
  FILE* file_;
  size_t position_;
  size_t length_;
};

} // namespace crypto
#endif // CRYPTO_FILE_INPUT_STREAM_H_
