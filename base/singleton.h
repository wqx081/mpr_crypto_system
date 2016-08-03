#ifndef BASE_SINGLETON_H_
#define BASE_SINGLETON_H_
#include "base/atomicops.h"
#include "base/once.h"

namespace base {

template<typename T>
class Singleton {
 public:
  static T* GetInstance() {
    base::OnceInit(&once_, &Singleton<T>::Init);
    return instance_;
  }
  static void ShutDown() {
    delete instance_;
    instance_ = nullptr;
  }

 private:
  static void Init() {
    instance_ = new T();
  }
  static OnceType once_;
  static T* instance_;
};

template<typename T>
OnceType Singleton<T>::once_;

template<typename T>
T* Singleton<T>::instance_ = nullptr;

} // namespace base
#endif // BASE_SINGLETON_H_
