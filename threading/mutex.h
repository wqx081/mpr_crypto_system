#ifndef THRIFT_CONCURRENCY_MUTEX_H_
#define THRIFT_CONCURRENCY_MUTEX_H_ 1

#include <cstdint>
#include <memory>

#include "base/macros.h"

namespace threading {

/**
 * A simple mutex class
 */
class Mutex {
 public:
  explicit Mutex(int type = DEFAULT_INITIALIZER);

  virtual ~Mutex() {}
  virtual void Lock() const;
  virtual bool TryLock() const;
  virtual bool TimedLock(int64_t milliseconds) const;
  virtual void Unlock() const;

  virtual bool IsLocked() const;

  void* GetUnderlyingImpl() const;

  static int DEFAULT_INITIALIZER;
  static int RECURSIVE_INITIALIZER;

 private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

class ReadWriteMutex {
public:
  ReadWriteMutex();
  virtual ~ReadWriteMutex() {}

  virtual void AcquireRead() const;
  virtual void AcquireWrite() const;

  virtual bool TimedRead(int64_t milliseconds) const;
  virtual bool TimedWrite(int64_t milliseconds) const;

  virtual bool AttemptRead() const;
  virtual bool AttemptWrite() const;

  virtual void Release() const;

private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};

class Guard {
 public:
  explicit Guard(const Mutex& value, int64_t timeout = 0) : mutex_(&value) {
    if (timeout == 0) {
      value.Lock();
    } else if (timeout < 0) {
      if (!value.TryLock()) {
        mutex_ = nullptr;
      }
    } else {
      if (!value.TimedLock(timeout)) {
        mutex_ = nullptr;
      }
    }
  }
  ~Guard() {
    release();
  }

  // Move constructor/assignment.
  Guard(Guard&& other) noexcept {
    *this = std::move(other);
  }
  Guard& operator=(Guard&& other) noexcept {
    if (&other != this) {
      release();
      using std::swap;
      swap(mutex_, other.mutex_);
    }
    return *this;
  }

  bool release() {
    if (!mutex_) {
      return false;
    }
    mutex_->Unlock();
    mutex_ = nullptr;
    return true;
  }

  explicit operator bool() const {
    return mutex_ != nullptr;
  }

 private:
  const Mutex* mutex_ = nullptr;

  DISALLOW_COPY_AND_ASSIGN(Guard);
};

// Can be used as second argument to RWGuard to make code more readable
// as to whether we're doing acquireRead() or acquireWrite().
enum RWGuardType {
  RW_READ = 0,
  RW_WRITE = 1,
};


class RWGuard {
 public:
  explicit RWGuard(const ReadWriteMutex& value, bool write = false,
                   int64_t timeout=0)
      : rw_mutex_(&value) {
    bool locked = true;
    if (write) {
      if (timeout) {
        locked = rw_mutex_->TimedWrite(timeout);
      } else {
        rw_mutex_->AcquireWrite();
      }
    } else {
      if (timeout) {
        locked = rw_mutex_->TimedRead(timeout);
      } else {
        rw_mutex_->AcquireRead();
      }
    }
    if (!locked) {
      rw_mutex_ = nullptr;
    }
  }
  RWGuard(const ReadWriteMutex& value, RWGuardType type, int64_t timeout = 0)
      : RWGuard(value, type == RW_WRITE, timeout) {
  }

  ~RWGuard() {
    release();
  }

  // Move constructor/assignment.
  RWGuard(RWGuard&& other) noexcept {
    *this = std::move(other);
  }
  RWGuard& operator=(RWGuard&& other) noexcept {
    if (&other != this) {
      release();
      using std::swap;
      swap(rw_mutex_, other.rw_mutex_);
    }
    return *this;
  }

  explicit operator bool() const {
    return rw_mutex_ != nullptr;
  }

  bool release() {
    if (rw_mutex_ == nullptr) return false;
    rw_mutex_->Release();
    rw_mutex_ = nullptr;
    return true;
  }

 private:
  const ReadWriteMutex* rw_mutex_ = nullptr;

  DISALLOW_COPY_AND_ASSIGN(RWGuard);
};


// A little hack to prevent someone from trying to do "Guard(m);"

#define Guard(m) [&]() { \
  static_assert(false, \
    "\"Guard(m);\" is invalid because the temporary Guard object is " \
    "destroyed at the end of the line, releasing the lock. " \
    "Replace it with \"Guard g(m);\"" \
  ); \
  return (m); \
}()

#define RWGuard(m) [&]() { \
  static_assert(false, \
    "\"RWGuard(m);\" is invalid because the temporary RWGuard object is " \
    "destroyed at the end of the line, releasing the lock. " \
    "Replace it with \"RWGuard g(m);\"" \
  ); \
  return (m); \
}()


} // namespace threading
#endif // #ifndef THRIFT_CONCURRENCY_MUTEX_H_
