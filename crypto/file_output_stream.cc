#include "crypto/file_output_stream.h"

namespace crypto {

FileOutputStream::FileOutputStream()
  : file_(nullptr) {}

FileOutputStream::~FileOutputStream() {
  Close();
}

bool FileOutputStream::Open(const char* file_name, const char* mode) {
  if (file_) {
    Close();
  }
  file_ = fopen(file_name, mode);
  if (file_ == nullptr) {
    return false;
  }
  return true;
}

void FileOutputStream::Close() {
  if (file_) {
    fclose(file_);
    file_ = nullptr;
  }
}

bool FileOutputStream::Flush() {
  if (fflush(file_) == 0) {
    return true;
  }
  return false;
}

int FileOutputStream::Write(ByteVector* buffer) {
  return Write(buffer, 0, buffer->size());
}

int FileOutputStream::Write(ByteVector* buffer, size_t offset, size_t len) {
  if (!file_) {
    return 0;
  }
  if ((int32_t) offset >= 0 && len > 0) {
    size_t n = fwrite(&((*buffer)[offset]), 1, len, file_);
    return n;
  }
  return 0;
}

int FileOutputStream::Write(Byte byte) {
  return fwrite(&byte, 1, 1, file_);
}

int FileOutputStream::Write(Byte* buffer, size_t offset, size_t len) {
  if (!file_) {
    return 0;
  }
  if ((int32_t) offset >= 0 && len > 0) {
    size_t n = fwrite(buffer, 1, len, file_);
    return n;
  }
  return 0;
}

std::string FileOutputStream::ToString() { return ""; }

} // namespace crypto
