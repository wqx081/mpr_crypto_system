#ifndef _THRIFT_CONCURRENCY_UTIL_H_
#define _THRIFT_CONCURRENCY_UTIL_H_ 1

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <chrono>


namespace threading {

/**
 * Utility methods
 *
 * This class contains basic utility methods for converting time formats,
 * and other common platform-dependent concurrency operations.
 * It should not be included in API headers for other concurrency library
 * headers, since it will, by definition, pull in all sorts of horrid
 * platform dependent crap.  Rather it should be included directly in
 * concurrency library implementation source.
 *
 * @version $Id:$
 */
class TimeUtil {
 public:

  static const int64_t NS_PER_S = 1000000000LL;
  static const int64_t US_PER_S = 1000000LL;
  static const int64_t MS_PER_S = 1000LL;

  static const int64_t NS_PER_MS = NS_PER_S / MS_PER_S;
  static const int64_t NS_PER_US = NS_PER_S / US_PER_S;
  static const int64_t US_PER_MS = US_PER_S / MS_PER_S;

  static void ToTimespec(struct timespec& result, int64_t value) {
    result.tv_sec = value / MS_PER_S; // ms to s
    result.tv_nsec = (value % MS_PER_S) * NS_PER_MS; // ms to ns
  }

  static void ToTimeval(struct timeval& result, int64_t value) {
    result.tv_sec = value / MS_PER_S; // ms to s
    result.tv_usec = (value % MS_PER_S) * US_PER_MS; // ms to us
  }

  static void ToTicks(int64_t& result, int64_t secs, int64_t oldTicks,
                      int64_t oldTicksPerSec, int64_t newTicksPerSec) {
    result = secs * newTicksPerSec;
    result += oldTicks * newTicksPerSec / oldTicksPerSec;

    int64_t oldPerNew = oldTicksPerSec / newTicksPerSec;
    if (oldPerNew && ((oldTicks % oldPerNew) >= (oldPerNew / 2))) {
      ++result;
    }
  }
  static void ToTicks(int64_t& result,
                      const struct timespec& value,
                      int64_t ticksPerSec) {
    return ToTicks(result, value.tv_sec, value.tv_nsec, NS_PER_S, ticksPerSec);
  }
  static void ToTicks(int64_t& result,
                      const struct timeval& value,
                      int64_t ticksPerSec) {
    return ToTicks(result, value.tv_sec, value.tv_usec, US_PER_S, ticksPerSec);
  }
  static void ToMilliseconds(int64_t& result,
                             const struct timespec& value) {
    return ToTicks(result, value, MS_PER_S);
  }
  static void ToMilliseconds(int64_t& result,
                             const struct timeval& value) {
    return ToTicks(result, value, MS_PER_S);
  }
  static void ToUsec(int64_t& result, const struct timespec& value) {
    return ToTicks(result, value, US_PER_S);
  }
  static void ToUsec(int64_t& result, const struct timeval& value) {
    return ToTicks(result, value, US_PER_S);
  }
  static int64_t CurrentTimeTicks(int64_t ticksPerSec);
  static int64_t CurrentTime() { return CurrentTimeTicks(MS_PER_S); }
  static int64_t CurrentTimeUsec() { return CurrentTimeTicks(US_PER_S); }
  static int64_t MonotonicTimeTicks(int64_t ticksPerSec);
  static int64_t MonotonicTime() { return MonotonicTimeTicks(MS_PER_S); }
  static int64_t MonotonicTimeUsec() {
    return MonotonicTimeTicks(US_PER_S);
  }
};

typedef std::chrono::system_clock SystemClock;
typedef std::chrono::time_point<SystemClock> SystemClockTimePoint;


} // namespace threading
#endif // #ifndef _THRIFT_CONCURRENCY_UTIL_H_
