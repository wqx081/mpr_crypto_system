#ifndef DB_COMMON_DB_REF_COUNTED_TRAITS_H_
#define DB_COMMON_DB_REF_COUNTED_TRAITS_H_

#include "base/ref_counted.h"

namespace db {

template<typename T>
struct DBRefCountedThreadSafeTraits {
  static void Destruct(const T* x) {
    T::Dispose(const_cast<T *>(x));
  }
};

} // namespace db
#endif // DB_COMMON_DB_REF_COUNTED_TRAITS_H_
