#include "threading/thread.h"
#include "threading/thread_factory.h"
#include "threading/monitor.h"
#include "threading/time_util.h"

#include <set>

#include <unistd.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

namespace threading {

class Task : public Runnable {
 public:
  Task() {}
  void Run() override {
    LOG(INFO) << "\t\t\tHello World"; 
  }
};

bool HelloWorldTest() {
  PosixThreadFactory thread_factory = PosixThreadFactory();
  std::shared_ptr<Task> task = std::shared_ptr<Task>(new Task());
  std::shared_ptr<Thread> thread = thread_factory.NewThread(task);

  thread->Start();
  thread->Join();

  LOG(INFO) << "\t\t\tSuccess!";
  return true;
}

TEST(ThreadFactory, HelloWorldTest) {
  EXPECT_TRUE(HelloWorldTest());
}

class RecordIdTask : public Runnable {
 public:
  explicit RecordIdTask(ThreadFactory* factory) : factory_(factory) {}
  void Run() override {
    id = factory_->GetCurrentThreadId();
  }

  Thread::id_t id;
  ThreadFactory* factory_;
};

bool GetCurrentThreadIdTest() {
  PosixThreadFactory thread_factory = PosixThreadFactory();
  thread_factory.SetDetached(false);

  std::shared_ptr<RecordIdTask> task = std::shared_ptr<RecordIdTask>(
    new RecordIdTask(&thread_factory));

  std::shared_ptr<Thread> thread = thread_factory.NewThread(task);

  thread->Start();
  Thread::id_t id = thread->GetId();
  thread->Join();

  EXPECT_EQ(id, task->id);
  LOG(INFO) << "\t\t\tSuccess!";
  return true;
}

TEST(ThreadFactory, GetCurrentThreadIdTest) {
  EXPECT_TRUE(GetCurrentThreadIdTest());
}


class ReapNTask : public Runnable {
 public:
  ReapNTask(Monitor& monitor, int& active_count) :
    monitor_(monitor),
    count_(active_count) {}

  void Run() override {
    Synchronized s(monitor_);
    count_--;

    if (count_ == 0) {
      monitor_.Notify();
    }
  }

  Monitor& monitor_;
  int& count_;
};

bool ReapNThreads(int loop = 1, int count=10) {
  PosixThreadFactory thread_factory = PosixThreadFactory();
  Monitor* monitor = new Monitor();

  for (int lix = 0; lix < loop; lix++) {
    int* active_count = new int(count);
    std::set<std::shared_ptr<Thread>> threads;
    int tix;

    for (tix = 0; tix < count; tix++) {
      threads.insert(thread_factory.NewThread(std::shared_ptr<Runnable>(
        new ReapNTask(*monitor, *active_count))));
    }

    tix = 0;
    for (std::set<std::shared_ptr<Thread>>::const_iterator thread = threads.begin();
         thread != threads.end();
	 ++thread) {
      (*thread) ->Start();
    }

    {
      Synchronized s(*monitor);
      while (*active_count > 0) {
        monitor->Wait(1000);
      }
    }

    LOG(INFO) << "\t\t\treaped" << lix * count << " threads";
  }

  LOG(INFO) << "\t\t\tSuccess!";
  return true;
}

TEST(ThreadFactory, ReapNThreadsTest) {
  EXPECT_TRUE(ReapNThreads(3, 100));
}

class SynchStartTask : public Runnable {
 public:
  enum STATE {
    UNINITIALIZED,
    STARTING,
    STARTED,
    STOPPING,
    STOPPED
  };

  SynchStartTask(Monitor& monitor, volatile STATE& state) :
    monitor_(monitor),
    state_(state) {}

  void Run() override {
    {
      Synchronized s(monitor_);
      if (state_ == SynchStartTask::STARTING) {
        state_ = SynchStartTask::STARTED;
	monitor_.Notify();
      }
    }

    {
      Synchronized s(monitor_);
      while (state_ == SynchStartTask::STARTED) {
        monitor_.Wait();
      }

      if (state_ == SynchStartTask::STOPPING) {
        state_ = SynchStartTask::STOPPED;
	monitor_.NotifyAll();
      }
    }
  }

 private:
  Monitor& monitor_;
  volatile STATE& state_;
};

bool SynchStartTaskTest() {

  Monitor monitor;
  SynchStartTask::STATE state = SynchStartTask::UNINITIALIZED;
  std::shared_ptr<SynchStartTask> task = std::shared_ptr<SynchStartTask>(
    new SynchStartTask(monitor, state));

  PosixThreadFactory thread_factory = PosixThreadFactory();
  std::shared_ptr<Thread> thread = thread_factory.NewThread(task);

  if (state == SynchStartTask::UNINITIALIZED) {
    state = SynchStartTask::STARTING;
    thread->Start();
  }

  {
    Synchronized s(monitor);
    while (state == SynchStartTask::STARTING) {
      monitor.Wait();
    }
  }

  EXPECT_TRUE(state != SynchStartTask::STARTING);

  {
    Synchronized s(monitor);

    try {
      monitor.Wait(100);
    } catch (...) {
    }

    if (state == SynchStartTask::STARTED) {
      state = SynchStartTask::STOPPING;
      monitor.Notify();
    }

    while (state == SynchStartTask::STOPPING) {
      monitor.Wait();
    }
  }

  EXPECT_EQ(state, SynchStartTask::STOPPED);
  return true;
}

TEST(ThreadFactory, SynchStartTaskTest) {
  EXPECT_TRUE(SynchStartTaskTest());
}


bool MonitorTimeoutTest(size_t count=1000, int64_t timeout=10) {
  Monitor monitor;
  int64_t start_time = TimeUtil::CurrentTime();

  for (size_t ix = 0; ix < count; ++ix) {
    Synchronized s(monitor);
    try {
      monitor.Wait(timeout);
    } catch (...) {
    }
  }

  int64_t end_time = TimeUtil::CurrentTime();
  double error = ((end_time - start_time) - (count * timeout)) /
	  (double)(count * timeout);

  if (error < 0.0) {
    error *= 1.0;
  }

  bool success = error < 0.20;

  LOG(INFO) << "\t\t\t" << (success ? "Success" : "Failure")
	  << "! expected time: " << count * timeout
	  << "ms elapsed time: " << end_time - start_time
	  << "ms error%: " << error * 100.0;
  return success;
}

TEST(ThreadFactory, MonitorTimeoutTest) {
  EXPECT_TRUE(MonitorTimeoutTest());
}

class FloodTask : public Runnable {
 public:
  FloodTask(const size_t id) : id_(id) {}
  ~FloodTask() override {
    if (id_ % 1000 == 0) {
      LOG(INFO) << "\t\tthread " << id_ << " done";
    }
  }

  void Run() override {
    if (id_ % 1000 == 0) {
      LOG(INFO) << "\t\tthread " << id_ << " started";
    }
    usleep(1);
  }
  const size_t id_;
};

void Foo(PosixThreadFactory* tf) {
  (void) tf;
}

bool FloodNTest(size_t loop = 1, size_t count=100000) {
  bool success = false;
  for (size_t lix = 0; lix < loop; ++lix) {
    PosixThreadFactory thread_factory = PosixThreadFactory();
    thread_factory.SetDetached(true);

    for (size_t tix = 0; tix < count; ++tix) {
      std::shared_ptr<FloodTask> task(new FloodTask(lix * count + tix));
      std::shared_ptr<Thread> thread = thread_factory.NewThread(task);
      thread->Start();
      usleep(1);
    }

    LOG(INFO) << "\t\t\tFlooded " << (lix + 1) * count << " threads";
    success = true;
  }
  return success;
}

TEST(ThreadFactory, FloodNTest) {
  EXPECT_TRUE(FloodNTest(1, 100000));
}


} // namespace threading
