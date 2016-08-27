//@Author: WangQixiang (wangqx at mpreader.com)
//@Date: 2016/08/27
#ifndef CRYPTO_MEMORY_INPUT_STREAM_H_
#define CRYPTO_MEMORY_INPUT_STREAM_H_

#include <string>
#include <memory>

#include "base/macros.h"
#include "crypto/input_stream.h"

namespace crypto {

class MemoryInputStream : public InputStream {
 public:
  MemoryInputStream();
  MemoryInputStream(const Byte* buffer, size_t len);
  MemoryInputStream(const std::string& str);
  virtual ~MemoryInputStream() override;

  // InputStream
  virtual size_t Available() override;
  virtual void Close() override;
  virtual Byte Read() override;
  virtual size_t Read(ByteVector* b) override;
  virtual size_t Read(ByteVector* b, size_t offset, size_t length) override;
  virtual void Reset() override;
  virtual int64_t Skip(int64_t n) override;

  // My methods
  // virtual bool Attach(const Byte* buffer, size_t length);
  // virtual bool CopyFrom();

 private:
  std::unique_ptr<Byte[]> data_;
  size_t position_;
  size_t length_;

  DISALLOW_COPY_AND_ASSIGN(MemoryInputStream);
};

} // namespace crypto
#endif // CRYPTO_MEMORY_INPUT_STREAM_H_
