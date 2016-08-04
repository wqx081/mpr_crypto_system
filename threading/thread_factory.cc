#include "threading/thread_factory.h"
#include "threading/mutex.h"
#include "threading/exception.h"

#include <assert.h>
#include <pthread.h>
#include <sys/resource.h>

#include <iostream>

#include <glog/logging.h>

namespace threading {

using std::shared_ptr;
using std::weak_ptr;

const PosixThreadFactory::POLICY PosixThreadFactory::kDefaultPolicy;
const PosixThreadFactory::PRIORITY PosixThreadFactory::kDefaultPriority;
const int PosixThreadFactory::kDefaultStackSizeMB;

// global set to track the ids of live threads.
std::set<pthread_t> live_thread_ids;
Mutex live_thread_id_mutex;

void AddLiveThreadId(pthread_t tid) {
  Guard g(live_thread_id_mutex);
  live_thread_ids.insert(tid);
}

void RemoveLiveThreadId(pthread_t tid) {
  Guard g(live_thread_id_mutex);
  live_thread_ids.erase(tid);
}

void GetLiveThreadIds(std::set<pthread_t>* tids) {
  Guard g(live_thread_id_mutex);
  for (std::set<pthread_t>::const_iterator it = live_thread_ids.begin();
       it != live_thread_ids.end(); 
       ++it) {
    tids->insert(*it);
  }
}


// push our given name upstream into pthreads
bool PthreadThread::UpdateName() {
  if (!pthread_ || name_.empty()) {
    return false;
  }
  return SetPosixThreadName(pthread_, name_);
}

PthreadThread::PthreadThread(int policy, int priority, int stackSize,
                             bool detached,
                             shared_ptr<Runnable> runnable) :
  pthread_(0),
  state_(uninitialized),
  policy_(policy),
  priority_(priority),
  stackSize_(stackSize),
  detached_(detached) {

  this->Thread::SetRunnable(runnable);
}

PthreadThread::~PthreadThread() {
  /* Nothing references this thread, if is is not detached, do a join
     now, otherwise the thread-id and, possibly, other resources will
     be leaked. */
  if(!detached_) {
    try {
      Join();
    } catch(...) {
      // We're really hosed.
    }
  }
  RemoveLiveThreadId(pthread_);
}

void PthreadThread::Start() {
  Guard g(state_lock_);

  if (state_ != uninitialized) {
    return;
  }

  pthread_attr_t thread_attr;
  if (pthread_attr_init(&thread_attr) != 0) {
      throw SystemResourceException("pthread_attr_init failed");
  }

  if(pthread_attr_setdetachstate(&thread_attr,
                                 detached_ ?
                                 PTHREAD_CREATE_DETACHED :
                                 PTHREAD_CREATE_JOINABLE) != 0) {
      throw SystemResourceException("pthread_attr_setdetachstate failed");
  }

  // Set thread stack size
  if (pthread_attr_setstacksize(&thread_attr, MB * stackSize_) != 0) {
    throw SystemResourceException("pthread_attr_setstacksize failed");
  }

  // Create reference
  shared_ptr<PthreadThread>* selfRef = new shared_ptr<PthreadThread>();
  *selfRef = self_.lock();

  state_ = starting;

  if (pthread_create(&pthread_, 
	             &thread_attr, 
		     ThreadMain, 
		     (void*)selfRef) != 0) {
    throw SystemResourceException("pthread_create failed");
  }

  // Now that we have a thread, we can set the name we've been given, if any.
  UpdateName();
  AddLiveThreadId(pthread_);
}

void PthreadThread::Join() {
  Guard g(state_lock_);
  STATE join_state = state_;


  if (!detached_ && join_state != uninitialized) {
    void* ignore;
    /* XXX
       If join fails it is most likely due to the fact
       that the last reference was the thread itself and cannot
       join.  This results in leaked threads and will eventually
       cause the process to run out of thread resources.
       We're beyond the point of throwing an exception.  Not clear how
       best to handle this. */
    int res = pthread_join(pthread_, &ignore);
    detached_ = (res == 0);
    if (res != 0) {
      LOG(ERROR) << "PthreadThread::join(): fail with code " << res;
    }
  } else {
    LOG(ERROR) << "PthreadThread::join(): detached thread";
  }
}

Thread::id_t PthreadThread::GetId() {
  return (Thread::id_t)pthread_;
}

shared_ptr<Runnable> PthreadThread::runnable() const {
  return Thread::runnable();
}

void PthreadThread::SetRunnable(shared_ptr<Runnable> value) {
  Thread::SetRunnable(value);
}

void PthreadThread::WeakRef(shared_ptr<PthreadThread> self) {
  assert(self.get() == this);
  self_ = weak_ptr<PthreadThread>(self);
}

bool PthreadThread::SetName(const std::string& name) {
  Guard g(state_lock_);
  name_ = name;
  return UpdateName();
}

void* PthreadThread::ThreadMain(void* arg) {
  shared_ptr<PthreadThread> thread = *(shared_ptr<PthreadThread>*)arg;
  delete reinterpret_cast<shared_ptr<PthreadThread>*>(arg);

  if (thread == nullptr) {
    return (void*)0;
  }

  // Using pthread_attr_setschedparam() at thread creation doesn't actually
  // change the new thread's priority for some reason... Other people on the
  // 'net also complain about it.  The solution is to set priority inside the
  // new thread's threadMain.
  if (thread->policy_ == SCHED_FIFO || thread->policy_ == SCHED_RR) {
    struct sched_param sched_param;
    sched_param.sched_priority = thread->priority_;
    int err =
      pthread_setschedparam(pthread_self(), thread->policy_, &sched_param);
    if (err != 0) {
      VLOG(1) << "pthread_setschedparam failed (are you root?) with error " <<
        err, strerror(err);
    }
  } else if (thread->policy_ == SCHED_OTHER) {
    if (setpriority(PRIO_PROCESS, 0, thread->priority_) != 0) {
      VLOG(1) << "setpriority failed (are you root?) with error " <<
        errno, strerror(errno);
    }
  }

  thread->runnable()->Run();

  return (void*)0;
}

int PosixThreadFactory::Impl::ToPthreadPolicy(POLICY policy) {
  switch (policy) {
  case OTHER:
    return SCHED_OTHER;
  case FIFO:
    return SCHED_FIFO;
  case ROUND_ROBIN:
    return SCHED_RR;
  }
  return SCHED_OTHER;
}

int PosixThreadFactory::Impl::ToPthreadPriority(
    POLICY policy, PRIORITY priority) {
  int pthread_policy = ToPthreadPolicy(policy);
  int min_priority = 0;
  int max_priority = 0;
  if (pthread_policy == SCHED_FIFO || pthread_policy == SCHED_RR) {
    min_priority = sched_get_priority_min(pthread_policy);
    max_priority = sched_get_priority_max(pthread_policy);
  } else if (pthread_policy == SCHED_OTHER) {
    min_priority = 19;
    max_priority = -20;
  }
  int quanta = HIGHEST - LOWEST;
  float stepsperquanta = (float)(max_priority - min_priority) / quanta;

  if (priority <= HIGHEST) {
    return (int)(min_priority + stepsperquanta * priority);
  } else {
    // should never get here for priority increments.
    assert(false);
    return (int)(min_priority + stepsperquanta * NORMAL);
  }
}

PosixThreadFactory::Impl::Impl(
  POLICY policy, PRIORITY priority, int stackSize, DetachState detached) :
  policy_(policy),
  priority_(priority),
  stackSize_(stackSize),
  detached_(detached) {}

shared_ptr<Thread> PosixThreadFactory::Impl::NewThread(
    const shared_ptr<Runnable>& runnable,
    PosixThreadFactory::DetachState detachState) const {
  shared_ptr<PthreadThread> result = shared_ptr<PthreadThread>(
      new PthreadThread(ToPthreadPolicy(policy_),
                        ToPthreadPriority(policy_, priority_), stackSize_,
                        detachState == DETACHED, runnable));
  result->WeakRef(result);
  runnable->SetThread(result);
  return result;
}

int PosixThreadFactory::Impl::GetStackSize() const {
  return stackSize_;
}

void PosixThreadFactory::Impl::SetStackSize(int value) {
  stackSize_ = value;
}

PosixThreadFactory::PRIORITY PosixThreadFactory::Impl::GetPriority()
    const {
  return priority_;
}

/**
 * Sets priority.
 *
 *  XXX
 *  Need to handle incremental priorities properly.
 */
void PosixThreadFactory::Impl::SetPriority(PRIORITY value) {
  priority_ = value;
}

PosixThreadFactory::DetachState PosixThreadFactory::Impl::GetDetachState()
    const {
  return detached_;
}

void PosixThreadFactory::Impl::SetDetachState(DetachState value) {
  detached_ = value;
}

Thread::id_t PosixThreadFactory::Impl::GetCurrentThreadId() const {
  return (Thread::id_t)pthread_self();
}


PosixThreadFactory::PosixThreadFactory(POLICY policy, PRIORITY priority,
                                       int stackSize, bool detached)
  : impl_(new PosixThreadFactory::Impl(policy, priority, stackSize,
                                       detached ? DETACHED : ATTACHED)) {
}

PosixThreadFactory::PosixThreadFactory(DetachState detached)
  : impl_(new PosixThreadFactory::Impl(kDefaultPolicy, kDefaultPriority,
                                       kDefaultStackSizeMB, detached)) {
}

shared_ptr<Thread> PosixThreadFactory::NewThread(
    const shared_ptr<Runnable>& runnable) const {
  return impl_->NewThread(runnable, impl_->GetDetachState());
}

shared_ptr<Thread> PosixThreadFactory::NewThread(
    const shared_ptr<Runnable>& runnable,
    DetachState detachState) const {
  return impl_->NewThread(runnable, detachState);
}

int PosixThreadFactory::GetStackSize() const { 
  return impl_->GetStackSize(); 
}

void PosixThreadFactory::SetStackSize(int value) { 
  impl_->SetStackSize(value); 
}

PosixThreadFactory::PRIORITY PosixThreadFactory::GetPriority() const { 
  return impl_->GetPriority(); 
}

void PosixThreadFactory::SetPriority(PosixThreadFactory::PRIORITY value) { 
  impl_->SetPriority(value); 
}

bool PosixThreadFactory::IsDetached() const {
  return impl_->GetDetachState() == DETACHED;
}

void PosixThreadFactory::SetDetached(bool value) {
  impl_->SetDetachState(value ? DETACHED : ATTACHED);
}

void PosixThreadFactory::SetDetached(DetachState value) {
  impl_->SetDetachState(value);
}

Thread::id_t PosixThreadFactory::GetCurrentThreadId() const { 
  return impl_->GetCurrentThreadId(); 
}

} // namespace threading
