#include "crypto/memory_output_stream.h"

#include <glog/logging.h>

namespace crypto {

MemoryOutputStream::MemoryOutputStream() {}

MemoryOutputStream::~MemoryOutputStream() {}

void MemoryOutputStream::Close() {}
bool MemoryOutputStream::Flush() { return true; }

int MemoryOutputStream::Write(ByteVector* buffer) {
  data_.insert(data_.end(), buffer->begin(), buffer->end());
  return buffer->size();
}

int MemoryOutputStream::Write(ByteVector* buffer,
                               size_t offset,
                               size_t length) {
  DCHECK(buffer);
  if ((int32_t)offset >= 0 && length > 0) {
    data_.insert(data_.end(),
                 buffer->begin() + offset,
                 buffer->begin() + offset + length);  
    return length;
  }
  return 0;
}

int MemoryOutputStream::Write(Byte* buffer, size_t offset, size_t length) {
  DCHECK(buffer);
  if ((int32_t) offset >= 0 && length > 0) {
    data_.insert(data_.end(),
                 buffer + offset,
                 buffer + offset + length);
    return length;
  }
  return 0;
}

int MemoryOutputStream::Write(Byte byte) {
  data_.push_back(byte);
  return 1;
}

Byte* MemoryOutputStream::data() {
  if (data_.empty()) {
    return nullptr;
  }
  return &(data_[0]);
}

size_t MemoryOutputStream::size() const {
  return data_.size();
}

std::string MemoryOutputStream::ToString() {
  return std::string((char *)data_.data(), data_.size());
}

} // namespace crypto
