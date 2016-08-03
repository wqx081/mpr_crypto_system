#ifndef THRIFT_CONCURRENCY_MUTEXIMPL_H_
#define THRIFT_CONCURRENCY_MUTEXIMPL_H_ 1

#include "threading/time_util.h"
#include "threading/mutex.h"

#include <glog/logging.h>
#include <pthread.h>
#include <errno.h>

namespace threading {

int Mutex::DEFAULT_INITIALIZER = PTHREAD_MUTEX_NORMAL;
int Mutex::RECURSIVE_INITIALIZER = PTHREAD_MUTEX_RECURSIVE;

/*
 * mutex implementations
 *
 * The motivation for these mutexes is that you can use the same template for
 * plain mutex and recursive mutex behavior. Behavior is defined at runtime.
 * Mostly, this exists for backwards compatibility in the
 * apache::thrift::concurrency::Mutex and ReadWriteMutex classes.
 */
class Mutex::Impl {
 public:
  explicit Impl(int type) {
    pthread_mutexattr_t mutexattr;
    CHECK(0 == pthread_mutexattr_init(&mutexattr));
    CHECK(0 == pthread_mutexattr_settype(&mutexattr, type));
    CHECK(0 == pthread_mutex_init(&pthread_mutex_, &mutexattr));
    CHECK(0 == pthread_mutexattr_destroy(&mutexattr));
  }
  ~Impl() {
    CHECK(0 == pthread_mutex_destroy(&pthread_mutex_));
  }

  void Lock() {
    int ret = pthread_mutex_lock(&pthread_mutex_);
    CHECK(ret != EDEADLK);
  }

  bool TryLock() { return (0 == pthread_mutex_trylock(&pthread_mutex_)); }

  template<class Rep, class Period>
  bool TryLockFor(const std::chrono::duration<Rep,Period>& timeout_duration) {
    auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        timeout_duration);
    struct timespec ts;
    TimeUtil::ToTimespec(ts, TimeUtil::CurrentTime() + durationMs.count());
    return 0 == pthread_mutex_timedlock(&pthread_mutex_, &ts);
  }

  void Unlock() {
    int ret = pthread_mutex_unlock(&pthread_mutex_);
    CHECK(ret != EPERM);
  }

  bool IsLocked() {
    // TODO: this doesn't work with recursive locks
    // We would probably need to track additional state to handle this
    // correctly for recursive locks.
    if (TryLock()) {
      Unlock();
      return false;
    }
    return true;
  }

  void* GetUnderlyingImpl() const { return (void*) &pthread_mutex_; }

 private:
   mutable pthread_mutex_t pthread_mutex_;
};

/**
 * Implementation of ReadWriteMutex class using POSIX rw lock
 */
class ReadWriteMutex::Impl {
 public:
  Impl() {
    CHECK(0 == pthread_rwlock_init(&rw_lock_, nullptr));
  }

  ~Impl() {
    CHECK(0 == pthread_rwlock_destroy(&rw_lock_));
  }

  void Lock() {
    int ret = pthread_rwlock_wrlock(&rw_lock_);
    CHECK(ret != EDEADLK);
  }

  bool TryLock() {
    return !pthread_rwlock_trywrlock(&rw_lock_);
  }

  template<class Rep, class Period>
  bool TryLockFor(const std::chrono::duration<Rep,Period>& timeout_duration) {
    auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        timeout_duration);
    struct timespec ts;
    TimeUtil::ToTimespec(ts, TimeUtil::CurrentTime() + durationMs.count());
    return 0 == pthread_rwlock_timedwrlock(&rw_lock_, &ts);
  }

  void Unlock() {
     pthread_rwlock_unlock(&rw_lock_);
  }

  void LockShared() {
    int ret = pthread_rwlock_rdlock(&rw_lock_);
    CHECK (ret != EDEADLK);
  }

  bool TryLockShared() {
    return !pthread_rwlock_tryrdlock(&rw_lock_);
  }

  template<class Rep, class Period>
  bool TryLockSharedFor(
      const std::chrono::duration<Rep,Period>& timeout_duration) {
    auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        timeout_duration);
    struct timespec ts;
    TimeUtil::ToTimespec(ts, TimeUtil::CurrentTime() + durationMs.count());
    return 0 == pthread_rwlock_timedrdlock(&rw_lock_, &ts);
  }

  void UnlockShared() {
    Unlock();
  }

 private:
  mutable pthread_rwlock_t rw_lock_;
};

///////////////////////////

Mutex::Mutex(int type) : impl_(std::make_shared<Mutex::Impl>(type)) {}
  
void* Mutex::GetUnderlyingImpl() const {
    return impl_->GetUnderlyingImpl();
}
  
void Mutex::Lock() const { impl_->Lock(); }
  
bool Mutex::TryLock() const { return impl_->TryLock(); }
  
bool Mutex::TimedLock(int64_t ms) const {
  return impl_->TryLockFor(std::chrono::milliseconds {ms});
}
  
void Mutex::Unlock() const { 
  impl_->Unlock(); 
}
  
bool Mutex::IsLocked() const { 
  return impl_->IsLocked(); 
}

///////////////////////////////
ReadWriteMutex::ReadWriteMutex()
    : impl_(std::make_shared<ReadWriteMutex::Impl>()) {}
  
void ReadWriteMutex::AcquireRead() const { impl_->LockShared(); }
  
void ReadWriteMutex::AcquireWrite() const { impl_->Lock(); }
  
bool ReadWriteMutex::TimedRead(int64_t milliseconds) const {
  return impl_->TryLockSharedFor(std::chrono::milliseconds {milliseconds});
}
  
bool ReadWriteMutex::TimedWrite(int64_t milliseconds) const {
  return impl_->TryLockFor(std::chrono::milliseconds {milliseconds});
}
  
bool ReadWriteMutex::AttemptRead() const { return impl_->TryLockShared(); }
  
bool ReadWriteMutex::AttemptWrite() const { return impl_->TryLock(); }
  
void ReadWriteMutex::Release() const { impl_->Unlock(); }


} // namespace threading

#endif // #ifndef THRIFT_CONCURRENCY_MUTEXIMPL_H_
