
#include "threading/monitor.h"
#include "threading/time_util.h"

#include <glog/logging.h>

#include <assert.h>
#include <errno.h>

#include <iostream>

#include <pthread.h>

namespace threading {

/**
 * Monitor implementation using the POSIX pthread library
 *
 */
class Monitor::Impl {

 public:

  Impl()
     : owned_mutex_(new Mutex()),
       mutex_(nullptr),
       condInitialized_(false) {
    Init(owned_mutex_.get());
  }

  Impl(Mutex* mutex)
     : mutex_(nullptr),
       condInitialized_(false) {
    Init(mutex);
  }

  Impl(Monitor* monitor)
     : mutex_(nullptr),
       condInitialized_(false) {
    Init(&(monitor->mutex()));
  }

  ~Impl() { Cleanup(); }

  Mutex& mutex() { return *mutex_; }
  void Lock() { mutex().Lock(); }
  void Unlock() { mutex().Unlock(); }

  /**
   * Exception-throwing version of waitForTimeRelative(), called simply
   * wait(int64) for historical reasons.  Timeout is in milliseconds.
   *
   * If the condition occurs,  this function returns cleanly; on timeout or
   * error an exception is thrown.
   */
  void Wait(int64_t timeout_ms) const {
    int result = WaitForTimeRelative(timeout_ms);
    if (result == ETIMEDOUT) {
      // pthread_cond_timedwait has been observed to return early on
      // various platforms, so comment out this assert.
      //assert(Util::currentTime() >= (now + timeout));
      //throw TimedOutException();
    } else if (result != 0) {
      //throw TLibraryException(
      //  "pthread_cond_wait() or pthread_cond_timedwait() failed");
    }
  }

  /**
   * Waits until the specified timeout in milliseconds for the condition to
   * occur, or waits forever if timeout_ms == 0.
   *
   * Returns 0 if condition occurs, ETIMEDOUT on timeout, or an error code.
   */
  int WaitForTimeRelative(int64_t timeout_ms) const {
    if (timeout_ms == 0LL) {
      return WaitForever();
    }

    struct timespec abstime;
    TimeUtil::ToTimespec(abstime, TimeUtil::CurrentTime() + timeout_ms);
    return WaitForTime(&abstime);
  }

  /**
   * Waits until the absolute time specified using struct timespec.
   * Returns 0 if condition occurs, ETIMEDOUT on timeout, or an error code.
   */
  int WaitForTime(const timespec* abstime) const {
    // The caller must lock the mutex before calling waitForTime()
    assert(mutex_);
    assert(mutex_->IsLocked());

    pthread_mutex_t* mutexImpl =
      reinterpret_cast<pthread_mutex_t*>(mutex_->GetUnderlyingImpl());
    assert(mutexImpl);

    return pthread_cond_timedwait(&pthread_cond_,
                                  mutexImpl,
                                  abstime);
  }

  /**
   * Waits forever until the condition occurs.
   * Returns 0 if condition occurs, or an error code otherwise.
   */
  int WaitForever() const {
    // The caller must lock the mutex before calling waitForever()
    assert(mutex_);
    assert(mutex_->IsLocked());

    pthread_mutex_t* mutexImpl =
      reinterpret_cast<pthread_mutex_t*>(mutex_->GetUnderlyingImpl());
    assert(mutexImpl);
    return pthread_cond_wait(&pthread_cond_, mutexImpl);
  }


  void Notify() {
    // The caller must lock the mutex before calling notify()
    assert(mutex_ && mutex_->IsLocked());

    int iret = pthread_cond_signal(&pthread_cond_);
    DCHECK(iret == 0);
  }

  void NotifyAll() {
    // The caller must lock the mutex before calling notify()
    assert(mutex_ && mutex_->IsLocked());

    int iret = pthread_cond_broadcast(&pthread_cond_);
    DCHECK(iret == 0);
  }

 private:

  void Init(Mutex* mutex) {
    mutex_ = mutex;

    if (pthread_cond_init(&pthread_cond_, nullptr) == 0) {
      condInitialized_ = true;
    }

    if (!condInitialized_) {
      Cleanup();
      //throw SystemResourceException();
    }
  }

  void Cleanup() {
    if (condInitialized_) {
      condInitialized_ = false;
      int iret = pthread_cond_destroy(&pthread_cond_);
      DCHECK(iret == 0);
    }
  }

  std::unique_ptr<Mutex> owned_mutex_;
  Mutex* mutex_;

  mutable pthread_cond_t pthread_cond_;
  mutable bool condInitialized_;
};

Monitor::Monitor() : impl_(new Monitor::Impl()) {}
Monitor::Monitor(Mutex* mutex) : impl_(new Monitor::Impl(mutex)) {}
Monitor::Monitor(Monitor* monitor) : impl_(new Monitor::Impl(monitor)) {}

Monitor::~Monitor() { delete impl_; }

Mutex& Monitor::mutex() const { return impl_->mutex(); }

void Monitor::Lock() const { impl_->Lock(); }

void Monitor::Unlock() const { impl_->Unlock(); }

void Monitor::Wait(int64_t timeout) const { impl_->Wait(timeout); }

int Monitor::WaitForTime(const timespec* abstime) const {
  return impl_->WaitForTime(abstime);
}

int Monitor::WaitForTimeRelative(int64_t timeout_ms) const {
  return impl_->WaitForTimeRelative(timeout_ms);
}

int Monitor::WaitForever() const {
  return impl_->WaitForever();
}

void Monitor::Notify() const { impl_->Notify(); }

void Monitor::NotifyAll() const { impl_->NotifyAll(); }

} // namespace threading
