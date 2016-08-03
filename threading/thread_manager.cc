#include "threading/thread_manager.h"
#include "threading/monitor.h"
#include "threading/time_util.h"

#include <memory>

#include <assert.h>
#include <queue>
#include <set>
#include <map>

#include <glog/logging.h>

namespace threading {

using std::shared_ptr;
using std::dynamic_pointer_cast;

/**
 * ThreadManager class
 *
 * This class manages a pool of threads. It uses a ThreadFactory to create
 * threads.  It never actually creates or destroys worker threads, rather
 * it maintains statistics on number of idle threads, number of active threads,
 * task backlog, and average wait and service times.
 *
 * @version $Id:$
 */
class ThreadManager::Impl : public ThreadManager {

public:
  Impl()
    : worker_count_(0),
      worker_max_count_(0),
      idle_count_(0),
      pending_task_count_max_(0),
      expired_count_(0),
      state_(ThreadManager::UNINITIALIZED),
      monitor_(&mutex_),
      max_monitor_(&mutex_) {}

  ~Impl() { Stop(); }
  void Start();
  void Stop() { StopImpl(false); }
  void Join() { StopImpl(true); }

  ThreadManager::STATE state() const { return state_; }

  shared_ptr<ThreadFactory> thread_factory() const {
    Synchronized s(monitor_);
    return thread_factory_;
  }

  void SetThreadFactory(shared_ptr<ThreadFactory> value) {
    Synchronized s(monitor_);
    thread_factory_ = value;
  }

  void AddWorker(size_t value);
  void RemoveWorker(size_t value);
  size_t IdleWorkerCount() const { return idle_count_; }
  size_t WorkerCount() const {
    Synchronized s(monitor_);
    return worker_count_;
  }
  size_t PendingTaskCount() const {
    Synchronized s(monitor_);
    return tasks_.size();
  }
  size_t TotalTaskCount() const {
    Synchronized s(monitor_);
    return tasks_.size() + worker_count_ - idle_count_;
  }

  size_t PendingTaskCountMax() const {
    Synchronized s(monitor_);
    return pending_task_count_max_;
  }

  size_t ExpiredTaskCount() {
    Synchronized s(monitor_);
    size_t result = expired_count_;
    expired_count_ = 0;
    return result;
  }

  void PendingTaskCountMax(const size_t value) {
    Synchronized s(monitor_);
    pending_task_count_max_ = value;
  }

  bool CanSleep();
  void Add(shared_ptr<Runnable> value, int64_t timeout, int64_t expiration);
  void Remove(shared_ptr<Runnable> task);
  shared_ptr<Runnable> RemoveNextPending();
  void RemoveExpiredTasks();
  void SetExpireCallback(ExpireCallback expireCallback);

private:
  void StopImpl(bool join);

  size_t worker_count_;
  size_t worker_max_count_;
  size_t idle_count_;
  size_t pending_task_count_max_;
  size_t expired_count_;
  ExpireCallback expire_callback_;

  ThreadManager::STATE state_;
  shared_ptr<ThreadFactory> thread_factory_;

  friend class ThreadManager::Task;
  std::queue<shared_ptr<Task> > tasks_;
  Mutex mutex_;
  Monitor monitor_;
  Monitor max_monitor_;
  Monitor worker_monitor_;

  friend class ThreadManager::Worker;
  std::set<shared_ptr<Thread> > workers_;
  std::set<shared_ptr<Thread> > dead_workers_;
  std::map<const Thread::id_t, shared_ptr<Thread> > id_map_;
};

class ThreadManager::Task : public Runnable {

public:
  enum STATE { WAITING, EXECUTING, CANCELLED, COMPLETE };

  Task(shared_ptr<Runnable> runnable, int64_t expiration = 0LL)
    : runnable_(runnable),
      state_(WAITING),
      expire_time_(expiration != 0LL 
		  ? TimeUtil::CurrentTime() + expiration : 0LL) {}

  ~Task() {}

  void Run() override {
    if (state_ == EXECUTING) {
      runnable_->Run();
      state_ = COMPLETE;
    }
  }

  shared_ptr<Runnable> GetRunnable() { return runnable_; }

  int64_t GetExpireTime() const { return expire_time_; }

private:
  shared_ptr<Runnable> runnable_;
  friend class ThreadManager::Worker;
  STATE state_;
  int64_t expire_time_;
};

class ThreadManager::Worker : public Runnable {
  enum STATE { UNINITIALIZED, STARTING, STARTED, STOPPING, STOPPED };

public:
  Worker(ThreadManager::Impl* manager) 
    : manager_(manager), state_(UNINITIALIZED), idle_(false) {}

  ~Worker() {}

private:
  bool IsActive() const {
    return (manager_->worker_count_ <= manager_->worker_max_count_)
           || (manager_->state_ == JOINING && !manager_->tasks_.empty());
  }

public:
  /**
   * Worker entry point
   *
   * As long as worker thread is running, pull tasks off the task queue and
   * execute.
   */
  void Run() override {
    bool active = false;
    /**
     * Increment worker semaphore and notify manager if worker count reached
     * desired max
     *
     * Note: We have to release the monitor and acquire the workerMonitor
     * since that is what the manager blocks on for worker add/remove
     */
    {
      bool notify_manager = false;
      {
        Synchronized s(manager_->monitor_);
        active = manager_->worker_count_ < manager_->worker_max_count_;
        if (active) {
          manager_->worker_count_++;
          notify_manager = manager_->worker_count_ 
		           == manager_->worker_max_count_;
        }
      }

      if (notify_manager) {
        Synchronized s(manager_->worker_monitor_);
        manager_->worker_monitor_.Notify();
      }
    }

    while (active) {
      shared_ptr<ThreadManager::Task> task;

      /**
       * While holding manager monitor block for non-empty task queue (Also
       * check that the thread hasn't been requested to stop). Once the queue
       * is non-empty, dequeue a task, release monitor, and execute. If the
       * worker max count has been decremented such that we exceed it, mark
       * ourself inactive, decrement the worker count and notify the manager
       * (technically we're notifying the next blocked thread but eventually
       * the manager will see it.
       */
      {
        Guard g(manager_->mutex_);
        active = IsActive();

        while (active && manager_->tasks_.empty()) {
          manager_->idle_count_++;
          idle_ = true;
          manager_->monitor_.Wait();
          active = IsActive();
          idle_ = false;
          manager_->idle_count_--;
        }

        if (active) {
          manager_->RemoveExpiredTasks();

          if (!manager_->tasks_.empty()) {
            task = manager_->tasks_.front();
            manager_->tasks_.pop();
            if (task->state_ == ThreadManager::Task::WAITING) {
              task->state_ = ThreadManager::Task::EXECUTING;
            }
          }

          /* If we have a pending task max and we just dropped below it, wakeup any
             thread that might be blocked on add. */
          if (manager_->pending_task_count_max_ != 0
              && manager_->tasks_.size() <= 
	         manager_->pending_task_count_max_ - 1) {
            manager_->max_monitor_.Notify();
          }
        }
      }

      if (task) {
        if (task->state_ == ThreadManager::Task::EXECUTING) {
          try {
            task->Run();
          } catch (...) {
            LOG(ERROR) << "task->run() raised an unknown exception";
          }
        }
      }
    }

    {
      Synchronized s(manager_->worker_monitor_);
      manager_->dead_workers_.insert(this->thread());
      idle_ = true;
      manager_->worker_count_--;
      bool notify_manager = (manager_->worker_count_ == 
		             manager_->worker_max_count_);
      if (notify_manager) {
        manager_->worker_monitor_.Notify();
      }
    }

    return;
  }

private:
  ThreadManager::Impl* manager_;
  friend class ThreadManager::Impl;
  STATE state_;
  bool idle_;
};

void ThreadManager::Impl::AddWorker(size_t value) {
  std::set<shared_ptr<Thread> > newThreads;
  for (size_t ix = 0; ix < value; ix++) {
    shared_ptr<ThreadManager::Worker> worker
        = shared_ptr<ThreadManager::Worker>(new ThreadManager::Worker(this));
    newThreads.insert(thread_factory_->NewThread(worker));
  }

  {
    Synchronized s(monitor_);
    worker_max_count_ += value;
    workers_.insert(newThreads.begin(), newThreads.end());
  }

  for (std::set<shared_ptr<Thread> >::iterator ix = newThreads.begin(); ix != newThreads.end();
       ++ix) {
    shared_ptr<ThreadManager::Worker> worker
        = dynamic_pointer_cast<ThreadManager::Worker, Runnable>((*ix)->runnable());
    worker->state_ = ThreadManager::Worker::STARTING;
    (*ix)->Start();
    id_map_.insert(std::pair<const Thread::id_t, shared_ptr<Thread> >((*ix)->GetId(), *ix));
  }

  {
    Synchronized s(worker_monitor_);
    while (worker_count_ != worker_max_count_) {
      worker_monitor_.Wait();
    }
  }
}

void ThreadManager::Impl::Start() {

  if (state_ == ThreadManager::STOPPED) {
    return;
  }

  {
    Synchronized s(monitor_);
    if (state_ == ThreadManager::UNINITIALIZED) {
      if (!thread_factory_) {
        //throw InvalidArgumentException();
      }
      state_ = ThreadManager::STARTED;
      monitor_.NotifyAll();
    }

    while (state_ == STARTING) {
      monitor_.Wait();
    }
  }
}

void ThreadManager::Impl::StopImpl(bool join) {
  bool doStop = false;
  if (state_ == ThreadManager::STOPPED) {
    return;
  }

  {
    Synchronized s(monitor_);
    if (state_ != ThreadManager::STOPPING && state_ != ThreadManager::JOINING
        && state_ != ThreadManager::STOPPED) {
      doStop = true;
      state_ = join ? ThreadManager::JOINING : ThreadManager::STOPPING;
    }
  }

  if (doStop) {
    RemoveWorker(worker_count_);
  }

  // XXX
  // should be able to block here for transition to STOPPED since we're no
  // using shared_ptrs

  {
    Synchronized s(monitor_);
    state_ = ThreadManager::STOPPED;
  }
}

void ThreadManager::Impl::RemoveWorker(size_t value) {
  std::set<shared_ptr<Thread> > removedThreads;
  {
    Synchronized s(monitor_);
    if (value > worker_max_count_) {
//      throw InvalidArgumentException();
    }

    worker_max_count_ -= value;

    if (idle_count_ < value) {
      for (size_t ix = 0; ix < idle_count_; ix++) {
        monitor_.Notify();
      }
    } else {
      monitor_.NotifyAll();
    }
  }

  {
    Synchronized s(worker_monitor_);

    while (worker_count_ != worker_max_count_) {
      worker_monitor_.Wait();
    }

    for (std::set<shared_ptr<Thread> >::iterator ix = dead_workers_.begin();
         ix != dead_workers_.end();
         ++ix) {
      id_map_.erase((*ix)->GetId());
      workers_.erase(*ix);
    }

    dead_workers_.clear();
  }
}

bool ThreadManager::Impl::CanSleep() {
  const Thread::id_t id = thread_factory_->GetCurrentThreadId();
  return id_map_.find(id) == id_map_.end();
}

void ThreadManager::Impl::Add(shared_ptr<Runnable> value, 
		              int64_t timeout, 
			      int64_t expiration) {
  LOG(INFO) << "timeout = " << timeout;
  Guard g(mutex_, timeout);
  LOG(INFO) << "---end timeout";

  if (!g) {
    DCHECK(false);
    //throw TimedOutException();
  }

  if (state_ != ThreadManager::STARTED) {
    //throw IllegalStateException(
    //    "ThreadManager::Impl::add ThreadManager "
    //    "not started");
  }

  LOG(INFO) << "------x";
  RemoveExpiredTasks();
  if (pending_task_count_max_ > 0 && 
      (tasks_.size() >= pending_task_count_max_)) {
    if (CanSleep() && timeout >= 0) {
      while (pending_task_count_max_ > 0 && 
	     tasks_.size() >= pending_task_count_max_) {
        // This is thread safe because the mutex is shared between monitors.
  LOG(INFO) << "------y";
        max_monitor_.Wait(timeout);
  LOG(INFO) << "------z";
      }
    } else {
   //   throw TooManyPendingTasksException();
    }
  }

  tasks_.push(shared_ptr<ThreadManager::Task>(new ThreadManager::Task(value, expiration)));

  // If idle thread is available notify it, otherwise all worker threads are
  // running and will get around to this task in time.
  if (idle_count_ > 0) {
    monitor_.Notify();
  }
}

void ThreadManager::Impl::Remove(shared_ptr<Runnable> task) {
  (void)task;
  Synchronized s(monitor_);
  if (state_ != ThreadManager::STARTED) {
//    throw IllegalStateException(
//        "ThreadManager::Impl::remove ThreadManager not "
//        "started");
  }
}

std::shared_ptr<Runnable> ThreadManager::Impl::RemoveNextPending() {
  Guard g(mutex_);
  if (state_ != ThreadManager::STARTED) {
//    throw IllegalStateException(
//        "ThreadManager::Impl::removeNextPending "
//        "ThreadManager not started");
  }

  if (tasks_.empty()) {
    return nullptr; //boost::shared_ptr<Runnable>();
  }

  shared_ptr<ThreadManager::Task> task = tasks_.front();
  tasks_.pop();

  return task->GetRunnable();
}

void ThreadManager::Impl::RemoveExpiredTasks() {
  int64_t now = 0LL; // we won't ask for the time untile we need it

  // note that this loop breaks at the first non-expiring task
  while (!tasks_.empty()) {
    shared_ptr<ThreadManager::Task> task = tasks_.front();
    if (task->GetExpireTime() == 0LL) {
      break;
    }
    if (now == 0LL) {
      now = TimeUtil::CurrentTime();
    }
    if (task->GetExpireTime() > now) {
      break;
    }
    if (expire_callback_) {
      expire_callback_(task->GetRunnable());
    }
    tasks_.pop();
    expired_count_++;
  }
}

void ThreadManager::Impl::SetExpireCallback(ExpireCallback expire_callback) {
  expire_callback_ = expire_callback;
}

class SimpleThreadManager : public ThreadManager::Impl {

public:
  SimpleThreadManager(size_t worker_count = 4, 
		      size_t pending_task_count_max = 0)
    : worker_count_(worker_count), 
      pending_task_count_max_(pending_task_count_max) {}

  void Start() {
    ThreadManager::Impl::PendingTaskCountMax(pending_task_count_max_);
    ThreadManager::Impl::Start();
    AddWorker(worker_count_);
  }

private:
  const size_t worker_count_;
  const size_t pending_task_count_max_;
  Monitor monitor_;
};

shared_ptr<ThreadManager> ThreadManager::NewThreadManager() {
  return shared_ptr<ThreadManager>(new ThreadManager::Impl());
}

shared_ptr<ThreadManager> ThreadManager::NewSimpleThreadManager(size_t count,
                                                                size_t pendingTaskCountMax) {
  return shared_ptr<ThreadManager>(new SimpleThreadManager(count, pendingTaskCountMax));
}

} // namespace threading
