#ifndef BASE_ATOMIC_REF_COUNT_H_
#define BASE_ATOMIC_REF_COUNT_H_

#include "base/atomicops.h"

namespace base {

typedef Atomic32 AtomicRefCount;

inline void AtomicRefCountIncN(volatile AtomicRefCount *ptr,
                               AtomicRefCount increment) {
  NoBarrier_AtomicIncrement(ptr, increment);
}

inline bool AtomicRefCountDecN(volatile AtomicRefCount *ptr,
                               AtomicRefCount decrement) {
  bool res = (Barrier_AtomicIncrement(ptr, -decrement) != 0);
  return res;
}

inline void AtomicRefCountInc(volatile AtomicRefCount *ptr) {
  base::AtomicRefCountIncN(ptr, 1);
}

inline bool AtomicRefCountDec(volatile AtomicRefCount *ptr) {
  return base::AtomicRefCountDecN(ptr, 1);
}

inline bool AtomicRefCountIsOne(volatile AtomicRefCount *ptr) {
  bool res = (Acquire_Load(ptr) == 1);
  return res;
}

inline bool AtomicRefCountIsZero(volatile AtomicRefCount *ptr) {
  bool res = (Acquire_Load(ptr) == 0);
  return res;
}


} // namespace base
#endif // BASE_ATOMIC_REF_COUNT_H_
