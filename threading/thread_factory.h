#ifndef THRIFT_CONCURRENCY_POSIXTHREADFACTORY_H_
#define THRIFT_CONCURRENCY_POSIXTHREADFACTORY_H_ 1

#include <set>
#include <string>


#include "threading/thread_name.h"
#include "threading/mutex.h"
#include "threading/thread.h"

#include <memory>

namespace threading {

void GetLiveThreadIds(std::set<pthread_t>* tids);


inline bool SetPosixThreadName(pthread_t id, const std::string& name) {
  return SetThreadName(id, name);
}

class PthreadThread : public Thread {
 public:

  enum STATE {
    uninitialized,
    starting,
  };

  static const int MB = 1024 * 1024;

  static void* ThreadMain(void* arg);

 protected:
  pthread_t pthread_;
  STATE state_;
  int policy_;
  int priority_;
  int stackSize_;
  std::weak_ptr<PthreadThread> self_;
  bool detached_;
  Mutex state_lock_;
  std::string name_;

  bool UpdateName();

 public:

  PthreadThread(int policy, int priority, int stackSize, bool detached,
                std::shared_ptr<Runnable> runnable);
  ~PthreadThread() override;

  void Start() override;
  void Join() override;
  Thread::id_t GetId() override;
  std::shared_ptr<Runnable> runnable() const override;
  void SetRunnable(std::shared_ptr<Runnable> value) override;
  void WeakRef(std::shared_ptr<PthreadThread> self);
  bool SetName(const std::string& name) override;
};

/**
 * A thread factory to create posix threads
 *
 * @version $Id:$
 */
class PosixThreadFactory : public ThreadFactory {

 public:

  /**
   * POSIX Thread scheduler policies
   */
  enum POLICY {
    OTHER,
    FIFO,
    ROUND_ROBIN
  };

  /**
   * POSIX Thread scheduler relative priorities,
   *
   * Absolute priority is determined by scheduler policy and OS. This
   * enumeration specifies relative priorities such that one can specify a
   * priority within a giving scheduler policy without knowing the absolute
   * value of the priority.
   */
  enum PRIORITY {
    LOWEST = 0,
    LOWER = 1,
    LOW = 2,
    NORMAL = 3,
    HIGH = 4,
    HIGHER = 5,
    HIGHEST = 6,
    INCREMENT = 7,
    DECREMENT = 8
  };

  static const POLICY kDefaultPolicy = OTHER;
  static const PRIORITY kDefaultPriority = NORMAL;
  static const int kDefaultStackSizeMB = 1;

  /**
   * Posix thread (pthread) factory.  All threads created by a factory are
   * reference-counted via std::shared_ptr and std::weak_ptr.  The factory
   * guarantees that threads and the Runnable tasks they host will be properly
   * cleaned up once the last strong reference to both is given up.
   *
   * Threads are created with the specified policy, priority, stack-size and
   * detachable-mode detached means the thread is free-running and will release
   * all system resources the when it completes.  A detachable thread is not
   * joinable.  The join method of a detachable thread will return immediately
   * with no error.
   *
   * By default threads are not joinable.
   */
  explicit PosixThreadFactory(POLICY policy=kDefaultPolicy,
                              PRIORITY priority=kDefaultPriority,
                              int stackSize=kDefaultStackSizeMB,
                              bool detached=true);

  explicit PosixThreadFactory(DetachState detached);

  // From ThreadFactory;
  std::shared_ptr<Thread> NewThread(
      const std::shared_ptr<Runnable>& runnable) const override;
  std::shared_ptr<Thread> NewThread(const std::shared_ptr<Runnable>& runnable,
                                    DetachState detachState) const override;

  // From ThreadFactory;
  Thread::id_t GetCurrentThreadId() const override;

  /**
   * Gets stack size for created threads
   *
   * @return int size in megabytes
   */
  virtual int GetStackSize() const;

  /**
   * Sets stack size for created threads
   *
   * @param value size in megabytes
   */
  virtual void SetStackSize(int value);

  /**
   * Gets priority relative to current policy
   */
  virtual PRIORITY GetPriority() const;

  /**
   * Sets priority relative to current policy
   */
  virtual void SetPriority(PRIORITY priority);

  /**
   * Sets detached mode of threads
   */
  virtual void SetDetached(bool detached);
  virtual void SetDetached(DetachState detached);

  /**
   * Gets current detached mode
   */
  virtual bool IsDetached() const;

  class Impl {
   protected:
    POLICY policy_;
    PRIORITY priority_;
    int stackSize_;
    DetachState detached_;

    static int ToPthreadPolicy(POLICY policy);
    static int ToPthreadPriority(POLICY policy, PRIORITY priority);

   public:

    Impl(POLICY policy, PRIORITY priority,
         int stackSize, DetachState detached);
    virtual ~Impl() {}

    virtual std::shared_ptr<Thread> NewThread(
      const std::shared_ptr<Runnable>& runnable,
      DetachState detachState) const;

    int GetStackSize() const;
    void SetStackSize(int value);
    PRIORITY GetPriority() const;

    void SetPriority(PRIORITY value);

    DetachState GetDetachState() const;
    void SetDetachState(DetachState value);
    Thread::id_t GetCurrentThreadId() const;

  };

 protected:
  std::shared_ptr<Impl> impl_;
};


} // namespace threading
#endif // #ifndef _THRIFT_CONCURRENCY_POSIXTHREADFACTORY_H_
