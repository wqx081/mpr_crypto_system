#include "threading/thread_manager.h"
#include "threading/thread_factory.h"
#include "threading/monitor.h"
#include "threading/time_util.h"

#include <limits>

#include <glog/logging.h>
#include <gtest/gtest.h>

using namespace threading;
using namespace base;

class LoadTask : public Runnable {
 public:
  LoadTask(Monitor* monitor, size_t* count, int64_t timeout)
    : monitor_(monitor),
      count_(count),
      timeout_(timeout),
      start_time_(0),
      end_time_(0) {}

  void Run() override {
    start_time_ = TimeUtil::CurrentTime();
    usleep(timeout_ * TimeUtil::US_PER_MS);
    end_time_ = TimeUtil::CurrentTime();

    {
      Synchronized s(*monitor_);

      (*count_)--;
      if (*count_ == 0) {
        monitor_->Notify();
      }
    }
  }

  Monitor* monitor_;
  size_t* count_;
  int64_t timeout_;
  int64_t start_time_;
  int64_t end_time_;
};


static void LoadTest(size_t num_tasks, int64_t timeout, size_t num_workers) {
  Monitor monitor;
  size_t tasks_left = num_tasks;

  auto thread_manager = ThreadManager::NewSimpleThreadManager(num_workers);
  auto thread_factory = std::make_shared<PosixThreadFactory>();
  thread_factory->SetPriority(PosixThreadFactory::HIGHEST);
  thread_manager->SetThreadFactory(thread_factory);
  thread_manager->Start();

  std::set<std::shared_ptr<LoadTask>> tasks;
  for (size_t n = 0; n < num_tasks; ++n) {
    tasks.insert(std::make_shared<LoadTask>(&monitor, &tasks_left, timeout));
  }

  int64_t start_time = TimeUtil::CurrentTime();
  for (const auto& task : tasks) {
    thread_manager->Add(task);
  }
  
  int64_t tasks_started_time = TimeUtil::CurrentTime();

  {
    Synchronized s(monitor);
    while (tasks_left > 0) {
      monitor.Wait();
    }
  }

  int64_t end_time = TimeUtil::CurrentTime();

  int64_t first_time = std::numeric_limits<int64_t>::max();
  int64_t last_time = 0;
  double average_time = 0.0;
  int64_t min_time = std::numeric_limits<int64_t>::max();
  int64_t max_time = 0;

  for (const auto& task : tasks) {
    EXPECT_GT(task->start_time_, 0);
    EXPECT_GT(task->end_time_, 0);

    int64_t delta = task->end_time_ - task->start_time_;
    assert(delta > 0);

    first_time = std::min(first_time, task->start_time_);
    last_time = std::max(last_time, task->end_time_);
    min_time = std::min(min_time, delta);
    max_time = std::max(max_time, delta);

    average_time += delta;
  }
  average_time /= num_tasks;

  LOG(INFO) << "first start: " << first_time << "ms "
            << "last end: " << last_time << "ms "
            << "min: " << min_time << "ms "
            << "max: " << max_time << "ms "
            << "average: " << average_time << " ms";

  double ideal_time = ((num_tasks + (num_workers - 1)) / num_workers) * timeout;
  double actual_time = end_time - start_time;
  double task_start_time = tasks_started_time - start_time;

  double overhead_pct = (actual_time - ideal_time) / ideal_time;
  if (overhead_pct < 0) {
    overhead_pct *= -1.0;
  }


  LOG(INFO) << "ideal time: " << ideal_time << "ms "
            << "actual time: "<< actual_time << "ms "
            << "task startup time: " << task_start_time << "ms "
            << "overhead: " << overhead_pct * 100.0 << "%";

  EXPECT_LT(overhead_pct, 0.10);

}

#if 0
TEST(ThreadManagerTest, LoadTest) {
  size_t num_tasks = 10000;
  int64_t timeout = 50;
  size_t num_workers = 100;
  LoadTest(num_tasks, timeout, num_workers);
}
#endif

////////////////
class BlockTask : public Runnable {
 public:
  BlockTask(Monitor& monitor, Monitor& bmonitor, size_t& count)
    : monitor_(monitor),
      bmonitor_(bmonitor),
      count_(count) {}

  void Run() override {
    {
      Synchronized s(bmonitor_);
      bmonitor_.Wait();
    }

    {
      Synchronized s(monitor_);
      count_--;
      //LOG(INFO) << "-------count: " << count_;
      if (count_ == 0) {
        monitor_.Notify();
      }
    }
  }

  Monitor& monitor_;  
  Monitor& bmonitor_;
  size_t& count_;
};

bool BlockTaskTest(int64_t /* timeout */, size_t num_workers) {
  bool success = false;

  try {
    Monitor bmonitor;
    Monitor monitor;
    
    size_t pending_task_max_count = num_workers;
    size_t active_counts[]{num_workers, pending_task_max_count, 1};
    std::shared_ptr<ThreadManager> thread_manager = 
      ThreadManager::NewSimpleThreadManager(num_workers, pending_task_max_count);
    std::shared_ptr<PosixThreadFactory> thread_factory = std::make_shared<PosixThreadFactory>();
    thread_factory->SetPriority(PosixThreadFactory::HIGHEST);
    thread_manager->SetThreadFactory(thread_factory);
    thread_manager->Start();
    
    std::set<std::shared_ptr<BlockTask>> tasks;
    for (size_t ix = 0; ix < num_workers; ++ix) {
      tasks.insert(std::make_shared<BlockTask>(monitor, bmonitor, active_counts[0]));
    }
    for (size_t ix = 0; ix < pending_task_max_count; ++ix) {
      tasks.insert(std::make_shared<BlockTask>(monitor, bmonitor, active_counts[1]));
    }
    for (std::set<std::shared_ptr<BlockTask>>::iterator it = tasks.begin();
         it != tasks.end();
         ++it) {
      thread_manager->Add(*it);
    }

    LOG(INFO) << "--------------------1";

    if (!(success = (thread_manager->TotalTaskCount() == pending_task_max_count + num_workers))) {
      //throw Exception("Unexpected pending task count");
      DCHECK(false);
    }

    LOG(INFO) << "--------------------2";
    std::shared_ptr<BlockTask> extra_task = std::make_shared<BlockTask>(monitor,
		    bmonitor,
		    active_counts[2]);

    try {
      thread_manager->Add(extra_task, 1);
    } catch (...) {
      LOG(ERROR) << "Should have timed out adding task in excess of pending task count";
    }

    LOG(INFO) << "--------------------3";

    try {
      thread_manager->Add(extra_task, -1);
    } catch (...) {
    }

    LOG(INFO) << "\t\t\t" << "Pending tasks " << thread_manager->PendingTaskCount();

    LOG(INFO) << "------------------------HERE a";

    {
      Synchronized s(bmonitor);
      bmonitor.NotifyAll();
    }

    LOG(INFO) << "------------------------HERE b";

    {
      Synchronized s(monitor);
      while (active_counts[0] != 0) {
        monitor.Wait();
      }
    }

    LOG(INFO) << "\t\t\t" << "Pending tasks " << thread_manager->PendingTaskCount();

    LOG(INFO) << "------------------------HERE 0";
    thread_manager->Add(extra_task, 1);
    LOG(INFO) << "-----------------------HERE 1";
    {
      Synchronized s(bmonitor);
      bmonitor.NotifyAll();
    }
    LOG(INFO) << "-----------------------HERE 2";

    {
      Synchronized s(monitor);
      while (active_counts[1] != 0) {
        monitor.Wait();
      }
    }
    LOG(INFO) << "-----------------------HERE 3";

    {
      Synchronized s(bmonitor);
      bmonitor.NotifyAll();
    }
    LOG(INFO) << "-----------------------HERE 3";

    {
      Synchronized s(monitor);
      while (active_counts[2] != 0) {
        monitor.Wait();
      }
    }
    LOG(INFO) << "-----------------------HERE 4";

    if (!(success = (thread_manager->TotalTaskCount() == 0))) {
    }

  } catch (...) {
    LOG(ERROR) << "ERROR: "; // << e.what();  
  }

  LOG(INFO) << "\t\t\t" << (success ? "Success" : "Failure");
  return success;          
}

TEST(ThreadManager, BlockTaskTest) {
  int64_t timeout = 50;
  size_t num_workers = 3000;
  EXPECT_TRUE(BlockTaskTest(timeout, num_workers));
}
