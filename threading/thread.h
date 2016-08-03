#ifndef _THRIFT_CONCURRENCY_THREAD_H_
#define _THRIFT_CONCURRENCY_THREAD_H_ 1

#include <stdint.h>
#include <memory>

namespace threading {

class Thread;

/**
 * Minimal runnable class.  More or less analogous to java.lang.Runnable.
 *
 */
class Runnable {

 public:
  virtual ~Runnable() {};
  virtual void Run() = 0;

  /**
   * Gets the thread object that is hosting this runnable object  - can return
   * an empty std::shared pointer if no references remain on the thread object
   */
  virtual std::shared_ptr<Thread> thread() { return thread_.lock(); }

  /**
   * Sets the thread that is executing this object.  This is only meant for
   * use by concrete implementations of Thread.
   */
  virtual void SetThread(std::shared_ptr<Thread> value) { thread_ = value; }

 private:
  std::weak_ptr<Thread> thread_;
};

enum PRIORITY {
  HIGH_IMPORTANT,
  HIGH,
  IMPORTANT,
  NORMAL,
  BEST_EFFORT,
  N_PRIORITIES
};

class PriorityRunnable : public virtual Runnable {
 public:
  virtual PRIORITY GetPriority() const = 0;
};

/**
 * Minimal thread class. Returned by thread factory bound to a Runnable object
 * and ready to start execution.  More or less analogous to java.lang.Thread
 * (minus all the thread group, priority, mode and other baggage, since that
 * is difficult to abstract across platforms and is left for platform-specific
 * ThreadFactory implementations to deal with
 */
class Thread {

 public:

  typedef uint64_t id_t;

  virtual ~Thread() {};

  virtual void Start() = 0;
  virtual void Join() = 0;
  virtual id_t GetId() = 0;
  virtual std::shared_ptr<Runnable> runnable() const { 
    return runnable_; 
  }
  virtual bool SetName(const std::string& name) { 
    (void) name;
    return false; 
  }

 protected:
  virtual void SetRunnable(std::shared_ptr<Runnable> value) { 
    runnable_ = value; 
  }

 private:
  std::shared_ptr<Runnable> runnable_;
};

/**
 * Factory to create platform-specific thread object and bind them to Runnable
 * object for execution
 */
class ThreadFactory {

 public:
  enum DetachState {
    ATTACHED,
    DETACHED
  };

  virtual ~ThreadFactory() {}
  virtual std::shared_ptr<Thread> NewThread(
      const std::shared_ptr<Runnable>& runnable) const = 0;

  virtual std::shared_ptr<Thread> NewThread(
      const std::shared_ptr<Runnable>& runnable,
      DetachState detachState) const = 0;

  /** Gets the current thread id or unknown_thread_id if the current thread is not a thrift thread */

  static const Thread::id_t unknown_thread_id;

  virtual Thread::id_t GetCurrentThreadId() const = 0;
};

} // namespace threading

#endif // #ifndef _THRIFT_CONCURRENCY_THREAD_H_
