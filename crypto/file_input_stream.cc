#include "crypto/file_input_stream.h"

namespace crypto {

FileInputStream::FileInputStream()
    : file_(nullptr),
      position_(0),
      length_(0) {
}

FileInputStream::~FileInputStream() {
  Close();
}

void FileInputStream::Close() {
  if (file_) {
    fclose(file_);
    length_ = 0;
    position_ = 0;
    file_ = nullptr;
  }
}

bool FileInputStream::Open(const char* file_name) {
  if (file_) {
    Close();
  }
  file_ = fopen(file_name, "rb");
  if (file_ == nullptr) {
    return false;
  }
  fseek(file_, 0, SEEK_END);
  length_ = ftell(file_);
  fseek(file_, 0, SEEK_SET);
  return true;
}

size_t FileInputStream::Available() {
  return length_ - position_;
}

Byte FileInputStream::Read() {
  if (!file_) {
    return 0;
  }
  if (feof(file_)) {
    return 0;
  }
  Byte value;
  size_t len = fread(&value, 1, 1, file_);
  position_ += len;
  return value;
}

size_t FileInputStream::Read(ByteVector* b) {
  return Read(b, 0, b->size());  
}

size_t FileInputStream::Read(ByteVector* b, size_t offset, size_t len) {
  if (!file_) {
    return 0;
  }
  if (feof(file_)) {
    return 0;
  }

  size_t read_count = std::min<size_t>(length_ - position_, len);  
  if (b->size() < (size_t)(offset + read_count)) {
    b->resize((size_t)(offset + read_count));
  }
  int n = fread(&((*b)[offset]), 1, read_count, file_);
  position_ += n;
  return n;
}

int64_t FileInputStream::Skip(int64_t n) {
  if (!Available()) {
    return 0;
  }
  int64_t skip_count = 0;
  if (n < 0) {
    skip_count = std::max<int64_t>(0 - (int64_t)position_, n);
    position_ -= (size_t)(0 - skip_count);
    fseek(file_, position_, SEEK_SET);
  } else {
    skip_count = std::min<size_t>(length_ - position_, (size_t)n);
    position_ += (size_t)skip_count;
    fseek(file_, (size_t)skip_count, SEEK_CUR);
  }
  return skip_count;
}

void FileInputStream::Reset() {}

} // namespace crypto
