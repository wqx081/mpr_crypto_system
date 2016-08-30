//@Author: WangQixiang (wangqx at mpreader.com)
//@Date: 2016/08/27
//
#ifndef CRYPTO_FILE_OUTPUT_STREAM_H_
#define CRYPTO_FILE_OUTPUT_STREAM_H_

#include <stdio.h>
#include "crypto/output_stream.h"

namespace crypto {

class FileOutputStream : public OutputStream {
 public:
  FileOutputStream();
  virtual ~FileOutputStream() override;

  // From OutputStream
  virtual void Close() override;
  virtual bool Flush() override;
  virtual int Write(ByteVector* buffer) override;
  virtual int Write(Byte n) override;
  virtual int Write(ByteVector* buffer, size_t offset, size_t len) override;
  virtual int Write(Byte* buffer, size_t offset, size_t len) override;
  virtual std::string ToString() override;

  //
  bool Open(const char* file_name, const char* mode = "w");

 private:
  FILE* file_;
};

} // namespace crypto
#endif // CRYPTO_FILE_OUTPUT_STREAM_H_
