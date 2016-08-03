#include "crypto/openssl_util.h"

#include "third_party/boringssl/include/openssl/crypto.h"
#include "third_party/boringssl/include/openssl/err.h"

#include <stddef.h>
#include <stdint.h>

#include <glog/logging.h>
#include "base/string_piece.h"

namespace crypto {

namespace {

int OpenSSLErrorCallback(const char* str, size_t len, void* context) {
  (void) context;
  DVLOG(1) << "\t" << base::StringPiece(str, len);
  return 1;
}

} // namespace

void EnsureOpenSSLInit() {
  CRYPTO_library_init();
}

void ClearOpenSSLERRStack(const base::Location& location) {
  //DEBUG
  uint32_t error_num = ERR_peek_error();
  if (error_num == 0) {
    return;
  }
  std::string message;
  location.Write(true, true, &message);
  DVLOG(1) << "OpenSSL ERR_get_error stack from " << message;
  ERR_print_errors_cb(&OpenSSLErrorCallback, nullptr);
  // NDEBUG
  // ERR_clear_error();
} 

} // namespace crypto
