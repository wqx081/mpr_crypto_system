//@Author: WangQixiang (wangqx at mpreader.com)
//@Date: 2016/08/27
#ifndef CRYPTO_MEMORY_OUTPUT_STREAM_H_
#define CRYPTO_MEMORY_OUTPUT_STREAM_H_
#include "base/macros.h"
#include "crypto/output_stream.h"

namespace crypto {

class MemoryOutputStream : public OutputStream {
 public:
  MemoryOutputStream();
  virtual ~MemoryOutputStream() override;

  // OutputStream
  virtual void Close() override;
  virtual void Flush() override;
  virtual void Write(ByteVector* buffer) override;
  virtual void Write(ByteVector* buffer, size_t offset, size_t length) override;
  virtual void Write(Byte* buffer, size_t offset, size_t length) override;
  virtual void Write(Byte b) override;

  virtual std::string ToString() override;

  Byte* data();
  size_t size() const;

 private:
  std::vector<Byte> data_;

  DISALLOW_COPY_AND_ASSIGN(MemoryOutputStream);
};


} // namespace crypto
#endif // CRYPTO_MEMORY_OUTPUT_STREAM_H_

