#include "base/once.h"
#include <sched.h>
#include "base/atomicops.h"

namespace base {

namespace {

void SchedYield() {
  sched_yield();
}

} // namespace

void OnceInitImpl(OnceType* once, std::function<void()> closure) {

  base::AtomicWord state = base::Acquire_Load(once);
  if (state == ONCE_STATE_DONE) {
    return;
  }

  state = base::Acquire_CompareAndSwap(once,
                                       ONCE_STATE_UNINITIALIZED,
                                       ONCE_STATE_EXECUTING_CLOSURE);
  if (state == ONCE_STATE_UNINITIALIZED) {
    // Closure() Run
    closure();
    base::Release_Store(once, ONCE_STATE_DONE); 
  } else {
    while (state == ONCE_STATE_EXECUTING_CLOSURE) {
      SchedYield();
      state = base::Acquire_Load(once);
    }
  }
}

} // namespace base
