#include "crypto/memory_input_stream.h"

#include <string.h>
#include <glog/logging.h>

namespace crypto {

MemoryInputStream::MemoryInputStream()
    : data_(nullptr), position_(0), length_(0) {}

MemoryInputStream::MemoryInputStream(const Byte* buffer, size_t len)
    : data_(new Byte[len]),
      position_(0),
      length_(len) {
  memcpy(data_.get(), buffer, len); 
}

MemoryInputStream::MemoryInputStream(const std::string& str)
    : data_(new Byte[str.size()]),
      position_(0),
      length_(str.size()) {
  memcpy(data_.get(), str.data(), length_);
}

MemoryInputStream::~MemoryInputStream() { 
  Close(); 
}

size_t MemoryInputStream::Available() {
  return length_ - position_;
}

void MemoryInputStream::Close() {
  // Nothing to do
}

Byte MemoryInputStream::Read() {
  if (!data_) {
    // throw  
    return 0;
  }
  if (position_ >= length_) {
    // throw
    return 0;
  }
  Byte value = data_[position_++];
  return value;
}

size_t MemoryInputStream::Read(ByteVector* b) {
  // 长度的指明放在 b->size() 是不是不太好???
  return Read(b, 0, b->size());
}

size_t MemoryInputStream::Read(ByteVector* b, size_t offset, size_t len) {
  DCHECK(b);
  if (!data_) {
    return 0;
  }
  if (position_ >= length_) {
    return 0;
  }
  size_t read_count = std::min<size_t>(length_ - position_, len);
  if (b->size() < (size_t)(offset + read_count)) {
    b->resize((size_t)(offset + read_count));
  }
  memcpy(&((*b)[offset]), data_.get() + position_, read_count);
  position_ += read_count;
  return read_count;
}

void MemoryInputStream::Reset() {
  // ???
  position_ = 0;
}

int64_t MemoryInputStream::Skip(int64_t n) {
  if (!data_) {
    return 0;
  }
  int64_t skip_count = 0;
  if (n < 0) { // Move to Backwards
    skip_count = std::max<int64_t>(0 - (int64_t)position_, n);
    position_ -= (size_t)(0 - skip_count);               
  } else {
    skip_count = std::min<size_t>(length_ - position_, (size_t)n);
    position_ += (size_t)skip_count;
  }
  return skip_count;
}

} // namespace crypto
