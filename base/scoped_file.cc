#include "base/scoped_file.h"
#include <glog/logging.h>
#include <unistd.h>
#include "base/eintr_wrapper.h"

namespace base {
namespace internal {

void ScopedFDCloseTraits::Free(int fd) {
  IGNORE_EINTR(close(fd));
}

} // namespace internal

} // namespace base
