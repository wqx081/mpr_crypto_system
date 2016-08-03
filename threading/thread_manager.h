#ifndef _THRIFT_CONCURRENCY_THREADMANAGER_H_
#define _THRIFT_CONCURRENCY_THREADMANAGER_H_ 1

#include "threading/thread.h"
#include <memory>
#include <functional>
#include <sys/types.h>

namespace threading {

/**
 * Thread Pool Manager and related classes
 */
class ThreadManager;

/**
 * ThreadManager class
 *
 * This class manages a pool of threads. It uses a ThreadFactory to create
 * threads. It never actually creates or destroys worker threads, rather
 * it maintains statistics on number of idle threads, number of active threads,
 * task backlog, and average wait and service times and informs the PoolPolicy
 * object bound to instances of this manager of interesting transitions. It is
 * then up the PoolPolicy object to decide if the thread pool size needs to be
 * adjusted and call this object addWorker and removeWorker methods to make
 * changes.
 *
 * This design allows different policy implementations to use this code to
 * handle basic worker thread management and worker task execution and focus on
 * policy issues. The simplest policy, StaticPolicy, does nothing other than
 * create a fixed number of threads.
 */
class ThreadManager {

protected:
  ThreadManager() {}

public:

  typedef std::function<void(std::shared_ptr<Runnable>)> ExpireCallback;

  virtual ~ThreadManager() {}

  /**
   * Starts the thread manager. Verifies all attributes have been properly
   * initialized, then allocates necessary resources to begin operation
   */
  virtual void Start() = 0;

  /**
   * Stops the thread manager. Aborts all remaining unprocessed task, shuts
   * down all created worker threads, and realeases all allocated resources.
   * This method blocks for all worker threads to complete, thus it can
   * potentially block forever if a worker thread is running a task that
   * won't terminate.
   */
  virtual void Stop() = 0;

  /**
   * Joins the thread manager. This is the same as stop, except that it will
   * block until all the workers have finished their work. At that point
   * the ThreadManager will transition into the STOPPED state.
   */
  virtual void Join() = 0;

  enum STATE { 
    UNINITIALIZED, 
    STARTING, 
    STARTED, 
    JOINING, 
    STOPPING, 
    STOPPED 
  };

  virtual STATE state() const = 0;

  virtual std::shared_ptr<ThreadFactory> thread_factory() const = 0;
  virtual void SetThreadFactory(std::shared_ptr<ThreadFactory> value) = 0;
  virtual void AddWorker(size_t value = 1) = 0;
  virtual void RemoveWorker(size_t value = 1) = 0;

  /**
   * Gets the current number of idle worker threads
   */
  virtual size_t IdleWorkerCount() const = 0;

  /**
   * Gets the current number of total worker threads
   */
  virtual size_t WorkerCount() const = 0;

  /**
   * Gets the current number of pending tasks
   */
  virtual size_t PendingTaskCount() const = 0;

  /**
   * Gets the current number of pending and executing tasks
   */
  virtual size_t TotalTaskCount() const = 0;

  /**
   * Gets the maximum pending task count.  0 indicates no maximum
   */
  virtual size_t PendingTaskCountMax() const = 0;

  /**
   * Gets the number of tasks which have been expired without being run.
   */
  virtual size_t ExpiredTaskCount() = 0;

  /**
   * Adds a task to be executed at some time in the future by a worker thread.
   *
   * This method will block if pendingTaskCountMax() in not zero and pendingTaskCount()
   * is greater than or equalt to pendingTaskCountMax().  If this method is called in the
   * context of a ThreadManager worker thread it will throw a
   * TooManyPendingTasksException
   *
   * @param task  The task to queue for execution
   *
   * @param timeout Time to wait in milliseconds to add a task when a pending-task-count
   * is specified. Specific cases:
   * timeout = 0  : Wait forever to queue task.
   * timeout = -1 : Return immediately if pending task count exceeds specified max
   * @param expiration when nonzero, the number of milliseconds the task is valid
   * to be run; if exceeded, the task will be dropped off the queue and not run.
   *
   * @throws TooManyPendingTasksException Pending task count exceeds max pending task count
   */
  virtual void Add(std::shared_ptr<Runnable> task,
                   int64_t timeout = 0LL,
                   int64_t expiration = 0LL) = 0;

  /**
   * Removes a pending task
   */
  virtual void Remove(std::shared_ptr<Runnable> task) = 0;

  /**
   * Remove the next pending task which would be run.
   *
   * @return the task removed.
   */
  virtual std::shared_ptr<Runnable> RemoveNextPending() = 0;

  /**
   * Remove tasks from front of task queue that have expired.
   */
  virtual void RemoveExpiredTasks() = 0;

  /**
   * Set a callback to be called when a task is expired and not run.
   *
   * @param expireCallback a function called with the shared_ptr<Runnable> for
   * the expired task.
   */
  virtual void SetExpireCallback(ExpireCallback expireCallback) = 0;

  static std::shared_ptr<ThreadManager> NewThreadManager();

  /**
   * Creates a simple thread manager the uses count number of worker threads and has
   * a pendingTaskCountMax maximum pending tasks. The default, 0, specified no limit
   * on pending tasks
   */
  static std::shared_ptr<ThreadManager> NewSimpleThreadManager(
		  size_t count = 4,
                  size_t pendingTaskCountMax = 0);

  class Task;
  class Worker;
  class Impl;
};


} // namespace threading
#endif // #ifndef _THRIFT_CONCURRENCY_THREADMANAGER_H_
