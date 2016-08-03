#include "threading/time_util.h"

#include <time.h>
#include <errno.h>
#include <assert.h>
#include <sys/time.h>

#include <glog/logging.h>

namespace threading {

int64_t TimeUtil::CurrentTimeTicks(int64_t ticksPerSec) {
  int64_t result;
  struct timespec now;
  int ret = clock_gettime(CLOCK_REALTIME, &now);
  DCHECK(ret == 0);
  ToTicks(result, now, ticksPerSec);

  return result;
}

int64_t TimeUtil::MonotonicTimeTicks(int64_t ticksPerSec) {
  static bool useRealtime;
  if (useRealtime) {
    return CurrentTimeTicks(ticksPerSec);
  }

  struct timespec now;
  int ret = clock_gettime(CLOCK_MONOTONIC, &now);
  if (ret != 0) {
    // CLOCK_MONOTONIC is probably not supported on this system
    assert(errno == EINVAL);
    useRealtime = true;
    return CurrentTimeTicks(ticksPerSec);
  }

  int64_t result;
  ToTicks(result, now, ticksPerSec);
  return result;
}


}
