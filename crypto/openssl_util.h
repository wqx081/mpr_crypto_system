#ifndef BASE_CRYPTO_OPENSSL_UTIL_H_
#define BASE_CRYPTO_OPENSSL_UTIL_H_

#include <stddef.h>

#include "base/macros.h"
#include "base/location.h"

namespace crypto {

template<int MIN_SIZE>
class ScopedOpenSSLSafeSizeBuffer {
 public:
  ScopedOpenSSLSafeSizeBuffer(unsigned char* output, size_t output_len)
    : output_(output),
      output_len_(output_len) {
  }

  ~ScopedOpenSSLSafeSizeBuffer() {
    if (output_len_ < MIN_SIZE) {
      memcpy(output_, min_sized_buffer_, output_len_);
    }
  }

  unsigned char* safe_buffer() {
    return output_len_ < MIN_SIZE ? min_sized_buffer_ : output_;
  }

 private:
  unsigned char* output_;
  size_t output_len_;

  unsigned char min_sized_buffer_[MIN_SIZE];

  DISALLOW_COPY_AND_ASSIGN(ScopedOpenSSLSafeSizeBuffer);
};

void EnsureOpenSSLInit();
void ClearOpenSSLERRStack(const base::Location& location);

class OpenSSLErrStackTracer {
 public:

  explicit OpenSSLErrStackTracer(const base::Location& location)
    : location_(location) {
    EnsureOpenSSLInit();
  }

  ~OpenSSLErrStackTracer() {
    ClearOpenSSLERRStack(location_);
  }

 private:
  const base::Location location_;
  
  OpenSSLErrStackTracer() = delete;
  OpenSSLErrStackTracer(const OpenSSLErrStackTracer&) = delete;
  OpenSSLErrStackTracer& operator=(const OpenSSLErrStackTracer&) = delete;
};

} // namespace crypto

#endif // BASE_CRYPTO_OPENSSL_UTIL_H_
