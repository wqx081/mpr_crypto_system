#include "threading/thread_manager.h"
#include "threading/thread_factory.h"
#include "threading/monitor.h"
#include "threading/time_util.h"
#include "threading/exception.h"

#include <limits>

#include <glog/logging.h>
#include <gtest/gtest.h>

#include <chrono>

using namespace threading;
using namespace base;

#define REQUIRE_EQUAL_TIMEOUT_(timeout, x, y) do { \
   using std::chrono::steady_clock; \
   using std::chrono::milliseconds;  \
   auto end = steady_clock::now() + milliseconds(timeout);  \
   while ((x) != (y) && steady_clock::now() < end)  {} \
   EXPECT_EQ(x, y); \
 } while (0)

#define REQUIRE_EQUAL_TIMEOUT(x, y) REQUIRE_EQUAL_TIMEOUT_(1000, x, y)

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

TEST(ThreadManagerTest, LoadTest) {
  size_t num_tasks = 10000;
  int64_t timeout = 50;
  size_t num_workers = 100;
  LoadTest(num_tasks, timeout, num_workers);
}

#if 0
////////////////
class BlockTask : public Runnable {
 public:
  BlockTask(Monitor* monitor, Monitor* bmonitor, bool* blocked, size_t* count)
    : monitor_(monitor),
      bmonitor_(bmonitor),
      blocked_(blocked),
      count_(count),
      started_(false) {}

  void Run() override {
    started_ = true;
    {
      Synchronized s(*bmonitor_);
      while (*blocked_) {
        bmonitor_->Wait();
      }
    }

    {
      Synchronized s(*monitor_);
      (*count_)--;
      if (*count_ == 0) {
        monitor_->Notify();
      }
    }
  }

  Monitor* monitor_;  
  Monitor* bmonitor_;
  bool* blocked_;
  size_t* count_;
  bool started_;
};

TEST(ThreadManager, BlockTaskTest) {
  const size_t num_workers = 8;
  size_t pending_task_max_count =  num_workers;
  
  auto thread_manager = ThreadManager::NewSimpleThreadManager(num_workers, pending_task_max_count);
  auto thread_factory = std::make_shared<PosixThreadFactory>();
  thread_manager->SetThreadFactory(thread_factory);
  thread_manager->Start();

  Monitor monitor;
  Monitor bmonitor;

  bool blocked1 = true;
  size_t tasks_count1 = num_workers;
  std::set<std::shared_ptr<BlockTask>> tasks;
  for (size_t ix = 0; ix < num_workers; ++ix) {
    auto task = std::make_shared<BlockTask>(&monitor,
                                            &bmonitor,
                                            &blocked1,
                                            &tasks_count1);
    tasks.insert(task);
    thread_manager->Add(task);
  }
  REQUIRE_EQUAL_TIMEOUT(thread_manager->TotalTaskCount(), num_workers); 
}
#endif
