#include "base/ref_counted.h"

namespace base {

bool RefCountedThreadSafeBase::HasOneRef() const {
  return AtomicRefCountIsOne(
    &const_cast<RefCountedThreadSafeBase*>(this)->ref_count_);
}

RefCountedThreadSafeBase::RefCountedThreadSafeBase()
  : ref_count_(0) {}

RefCountedThreadSafeBase::~RefCountedThreadSafeBase() {}

void RefCountedThreadSafeBase::AddRef() const {
  AtomicRefCountInc(&ref_count_);
}

bool RefCountedThreadSafeBase::Release() const {
  if (!AtomicRefCountDec(&ref_count_)) {
    return true;
  }
  return false;
}

} // namespace base
