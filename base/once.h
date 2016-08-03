#ifndef BASE_ONCE_H_
#define BASE_ONCE_H_
#include "base/atomicops.h"
#include <functional>

namespace base {

enum {
  ONCE_STATE_UNINITIALIZED = 0,
  ONCE_STATE_EXECUTING_CLOSURE = 1,
  ONCE_STATE_DONE = 2
};

typedef AtomicWord OnceType;

#define ONCE_INIT base::ONCE_STATE_UNINITIALIZED

void OnceInitImpl(OnceType* once, std::function<void()> closure);

inline void OnceInit(OnceType* once, void (*init_func)()) {
  if (base::Acquire_Load(once) != ONCE_STATE_DONE) {
    std::function<void()> func = init_func;
    OnceInitImpl(once, func);
  }
}

template<typename Arg>
inline void OnceInit(OnceType* once, void (*init_func)(Arg*), Arg* arg) {
  if (base::Acquire_Load(once) != ONCE_STATE_DONE) {
    std::function<void()> func = std::bind(init_func, arg);  
    OnceInitImpl(once, func);
  }
}

class OnceDynamic {
 public:
  OnceDynamic() : state_(ONCE_INIT) {}

  template<typename T>
  void Init(void (*func_with_arg), T* arg) {
    OnceInit<T>(&this->state_,
                func_with_arg,
                arg);
  }

 private:
  OnceType state_;
};

#define DECLARE_ONCE(NAME) \
  ::base::OnceType NAME = ONCE_INIT

} // namespace
#endif // BASE_ONCE_H_
