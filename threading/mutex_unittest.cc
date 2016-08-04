#include "threading/mutex.h"
#include "threading/time_util.h"

#include <gtest/gtest.h>

#include <thread>
#include <condition_variable>
#include <vector>

const int kTimeoutUsec = 10 * threading::TimeUtil::US_PER_MS;
const int kTimeoutMs = kTimeoutUsec / threading::TimeUtil::US_PER_MS;
const int kMaxReaders = 10;
const int kMicroSecInMilliSec = 1000;
const int kOpTimeInMs = 200;

TEST(ReadWriteMutex, Max_Readers) {
  threading::ReadWriteMutex l;
  for (int i=0; i < kMaxReaders; ++i) {
    EXPECT_TRUE(l.TimedRead(kTimeoutMs));
  }

  EXPECT_TRUE(l.TimedRead(kTimeoutMs));
}

TEST(ReadWriteMutex, Writer_Wait_Readers) {
  threading::ReadWriteMutex l;

  for (int i = 0; i < kMaxReaders; ++i) {
    EXPECT_TRUE(l.TimedRead(kTimeoutMs));
    EXPECT_FALSE(l.TimedWrite(kTimeoutMs));
  }

  for (int i = 0; i < kMaxReaders; ++i) {
    EXPECT_FALSE(l.TimedWrite(kTimeoutMs));
    l.Release();
  }

  EXPECT_TRUE(l.TimedWrite(kTimeoutMs));
  l.Release();

  std::vector<std::thread> threads_;
  for (int i = 0; i < kMaxReaders; ++i) {
    threads_.push_back(std::thread([this, &l] {
      EXPECT_TRUE(l.TimedRead(kTimeoutMs));
      usleep(kOpTimeInMs * kMicroSecInMilliSec);
      l.Release();
    }));
  }
  usleep(1000);

  std::thread thread1 = std::thread([this, &l] {
    EXPECT_FALSE(l.TimedWrite(0.5 * kOpTimeInMs));
  });

  std::thread thread2 = std::thread([this, &l] {
    EXPECT_TRUE(l.TimedWrite(1.5 * kOpTimeInMs));
    l.Release();
  });

  for (auto& t : threads_) {
    t.join(); 
  }
  thread1.join();
  thread2.join();
}

TEST(RWMutexTest, Readers_Wait_Writer) {
  threading::ReadWriteMutex l;
  
  EXPECT_TRUE(l.TimedWrite(kTimeoutMs));
  
  for (int i = 0; i < kMaxReaders; ++i) {
    EXPECT_FALSE(l.TimedRead(kTimeoutMs));
  }
  
  l.Release();
  
  for (int i = 0; i < kMaxReaders; ++i) {
    EXPECT_TRUE(l.TimedRead(kTimeoutMs));
  }
  
  for (int i = 0; i < kMaxReaders; ++i) {
    l.Release();
  }
  
  std::thread wrThread = std::thread([&l] {
    EXPECT_TRUE(l.TimedWrite(kTimeoutMs));
    usleep(kOpTimeInMs * kMicroSecInMilliSec);
    l.Release();
  });
  
  usleep(1000);
  
  std::vector<std::thread> threads_;
  for (int i = 0; i < kMaxReaders; ++i) {
    threads_.push_back(std::thread([&l] {
    EXPECT_FALSE(l.TimedRead(0.5 * kOpTimeInMs));
  }));
  
  threads_.push_back(std::thread([&l] {
    EXPECT_TRUE(l.TimedRead(1.5 * kOpTimeInMs));
    l.Release();
  }));
  } 
    
  for (auto& t : threads_) {
    t.join();
  } 
  wrThread.join();
} 

TEST(RWMutexTest, Writer_Wait_Writer) {
  threading::ReadWriteMutex l;
    
  EXPECT_TRUE(l.TimedWrite(kTimeoutMs));
  EXPECT_FALSE(l.TimedWrite(kTimeoutMs));
  l.Release();

  EXPECT_TRUE(l.TimedWrite(kTimeoutMs));
  EXPECT_FALSE(l.TimedWrite(kTimeoutMs));
  l.Release();
  
  std::thread wrThread1 = std::thread([this, &l] {
        EXPECT_TRUE(l.TimedWrite(kTimeoutMs));
        usleep(kOpTimeInMs * kMicroSecInMilliSec);
        l.Release();
  });
  
  usleep(1000);
  
  std::thread wrThread2 = std::thread([this, &l] {
        EXPECT_FALSE(l.TimedWrite(0.5 * kOpTimeInMs));
  });
      
  std::thread wrThread3 = std::thread([this, &l] {
        EXPECT_TRUE(l.TimedWrite(1.5 * kOpTimeInMs));
        l.Release();
  });
      
  wrThread1.join();
  wrThread2.join();
  wrThread3.join();
} 

TEST(RWMutexTest, Read_Holders) {
  threading::ReadWriteMutex l;
    
  threading::RWGuard guard(l, false);
  EXPECT_FALSE(l.TimedWrite(kTimeoutMs));
  EXPECT_TRUE(l.TimedRead(kTimeoutMs));
  l.Release();
  EXPECT_FALSE(l.TimedWrite(kTimeoutMs));
} 
  
TEST(RWMutexTest, Write_Holders) {
  threading::ReadWriteMutex l;
    
  threading::RWGuard guard(l, true);
  EXPECT_FALSE(l.TimedWrite(kTimeoutMs));
  EXPECT_FALSE(l.TimedRead(kTimeoutMs));
} 
  
TEST(MutexTest, Recursive_Holders) {
  threading::Mutex mutex(threading::Mutex::RECURSIVE_INITIALIZER);
  threading::Guard g1(mutex);
  {
    threading::Guard g2(mutex);
  }
  threading::Guard g2(mutex);
}

