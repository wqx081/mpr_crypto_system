#ifndef THRIFT_CONCURRENCY_MONITOR_H_
#define THRIFT_CONCURRENCY_MONITOR_H_ 1

#include "base/macros.h"
#include "threading/mutex.h"

namespace threading {

/**
 * A monitor is a combination mutex and condition-event.  Waiting and
 * notifying condition events requires that the caller own the mutex.  Mutex
 * lock and unlock operations can be performed independently of condition
 * events.  This is more or less analogous to java.lang.Object multi-thread
 * operations.
 *
 * Note the Monitor can create a new, internal mutex; alternatively, a
 * separate Mutex can be passed in and the Monitor will re-use it without
 * taking ownership.  It's the user's responsibility to make sure that the
 * Mutex is not deallocated before the Monitor.
 *
 * Note that all methods are const.  Monitors implement logical constness, not
 * bit constness.  This allows const methods to call monitor methods without
 * needing to cast away constness or change to non-const signatures.
 *
 */
class Monitor {
 public:
  Monitor();

  explicit Monitor(Mutex* mutex);
  explicit Monitor(Monitor* monitor);

  virtual ~Monitor();
  Mutex& mutex() const;
  virtual void Lock() const;
  virtual void Unlock() const;

  int WaitForTimeRelative(int64_t timeout_ms) const;
  int WaitForTime(const timespec* abstime) const;
  int WaitForever() const;
  void Wait(int64_t timeout_ms = 0LL) const;

  virtual void Notify() const;
  virtual void NotifyAll() const;

 private:

  class Impl;

  Impl* impl_;
  
  DISALLOW_COPY_AND_ASSIGN(Monitor);
};

class Synchronized {
 public:
 explicit Synchronized(const Monitor* monitor) : g(monitor->mutex()) { }
 explicit Synchronized(const Monitor& monitor) : g(monitor.mutex()) { }

 private:
  Guard g;
};

} // namespace threading

#endif // #ifndef THRIFT_CONCURRENCY_MONITOR_H_
