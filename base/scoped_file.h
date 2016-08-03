#ifndef SPARROW_FILES_SCOPED_FILE_H_
#define SPARROW_FILES_SCOPED_FILE_H_

#include <stdio.h>
#include <memory>
#include <glog/logging.h>
#include "base/scoped_generic.h"

namespace base {

namespace internal {

struct ScopedFDCloseTraits {
  static int InvalidValue() {
    return -1;
  }
  static void Free(int fd);
};

struct ScopedFILECloser {
  inline void operator()(FILE* x) const {
    if (x)
      fclose(x);
  }
};

} // namespace internal

typedef ScopedGeneric<int, internal::ScopedFDCloseTraits> ScopedFD;
typedef std::unique_ptr<FILE, internal::ScopedFILECloser> ScopedFILE;

} // namespace base
#endif // SPARROW_FILES_SCOPED_FILE_H_
