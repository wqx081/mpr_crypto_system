#include <unistd.h>
#include <pthread.h>
#include <functional>
#include "base/once.h"
#include <mutex>
#include <thread>

#include <glog/logging.h>
#include <gtest/gtest.h>

using namespace base;

class OnceInitTest : public ::testing::Test {
 public:
  enum State {
    INIT_NOT_STARTED,
    INIT_STARTED,
    INIT_DONE
  };

  void SetUp() {
    state_ = INIT_NOT_STARTED;
    current_test_ = this;
  }
  void SetOnces(OnceType* once, OnceType* recursive_once) {
    once_ = once;
    recursive_once_ = recursive_once;
  }

  void InitOnce() {
    base::OnceInit(once_, &InitStatic);
  }
  void InitRecursiveOnce() {
    base::OnceInit(recursive_once_, &InitRecursiveStatic);
  }

  void BlockInit() {
    init_blocker_.lock();
  }
  void UnblockInit() {
    init_blocker_.unlock();
  }
  
  class TestThread {
   public:
    TestThread(std::function<void()> callback)
      : done_(false), joined_(false), callback_(callback) {
      pthread_create(&thread_, nullptr, &Start, this);
    }
    ~TestThread() {
      if (!joined_) Join();
    }
    bool IsDone() {
      std::lock_guard<std::mutex> lock(done_mutex_);
      return done_;
    }
    void Join() {
      joined_ = true;
      pthread_join(thread_, nullptr);
    }

   private:
    pthread_t thread_;
    std::mutex done_mutex_;
    bool done_;
    bool joined_;
    std::function<void()> callback_;   

    static void* Start(void* arg) {
      reinterpret_cast<TestThread*>(arg)->Run();
      return 0;
    }
    void Run() {
       callback_();
      std::lock_guard<std::mutex> lock(done_mutex_);
      done_ = true;
    }
  };

  TestThread* RunInitOnceInNewThread() {
    std::function<void()> callback = std::bind(&OnceInitTest::InitOnce, this);
    return new TestThread(callback);
  }
  TestThread* RunInitRecursiveOnceInNewThread() {
    std::function<void()> callback = std::bind(&OnceInitTest::InitRecursiveOnce, this);
    return new TestThread(callback);
  }

  State CurrentState() {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_;
  }

  void WaitABit() {
    sleep(1);
  }

 private:
  std::mutex mutex_;
  std::mutex init_blocker_;
  State state_;
  OnceType* once_;
  OnceType* recursive_once_;

  void Init() {
    std::lock_guard<std::mutex> lock(mutex_);
    EXPECT_EQ(INIT_NOT_STARTED, state_);
    state_ = INIT_STARTED;
    mutex_.unlock();
    init_blocker_.lock();
    init_blocker_.unlock();
    mutex_.lock();
    state_ = INIT_DONE;
  }

  static OnceInitTest* current_test_;
  static void InitStatic() { current_test_->Init(); }
  static void InitRecursiveStatic() { current_test_->InitOnce(); }
};

OnceInitTest* OnceInitTest::current_test_ = nullptr;

DECLARE_ONCE(simple_once);

TEST_F(OnceInitTest, Simple) {
  SetOnces(&simple_once, nullptr);
  
  EXPECT_EQ(INIT_NOT_STARTED, CurrentState());
  InitOnce();
  EXPECT_EQ(INIT_DONE, CurrentState());

  InitOnce();
  EXPECT_EQ(INIT_DONE, CurrentState());
}

DECLARE_ONCE(recursive_once1);
DECLARE_ONCE(recursive_once2);

TEST_F(OnceInitTest, Recursive) {

  SetOnces(&recursive_once1, &recursive_once2);

  EXPECT_EQ(INIT_NOT_STARTED, CurrentState());
  InitRecursiveOnce();
  EXPECT_EQ(INIT_DONE, CurrentState());
}

DECLARE_ONCE(multiple_threads_once);

TEST_F(OnceInitTest, MultipleThreads) {
  
  SetOnces(&multiple_threads_once, nullptr);

  std::unique_ptr<TestThread> threads[4];
  EXPECT_EQ(INIT_NOT_STARTED, CurrentState());
  for (int i = 0; i < 4; ++i) {
    threads[i].reset(RunInitOnceInNewThread());
  }
  for (int i = 0; i < 4; ++i) {
    threads[i]->Join();
  }
  EXPECT_EQ(INIT_DONE, CurrentState());
}

DECLARE_ONCE(multiple_threads_blocked_once1);
DECLARE_ONCE(multiple_threads_blocked_once2);

TEST_F(OnceInitTest, MultipleThreadsBlocked) {

  SetOnces(&multiple_threads_blocked_once1, &multiple_threads_blocked_once2);

  std::unique_ptr<TestThread> threads[8];
  EXPECT_EQ(INIT_NOT_STARTED, CurrentState());

  BlockInit();
  for (int i = 0; i < 4; i++) {
    threads[i].reset(RunInitOnceInNewThread());
  }
  for (int i = 4; i < 8; i++) {
    threads[i].reset(RunInitRecursiveOnceInNewThread());
  }

  WaitABit();

  EXPECT_EQ(INIT_STARTED, CurrentState());
  UnblockInit();

  for (int i = 0; i < 8; i++) {
    threads[i]->Join();
  }
  EXPECT_EQ(INIT_DONE, CurrentState());

}
