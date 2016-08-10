#include "base/time.h"

#include <glog/logging.h>

#include <time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>

#include <nspr/prtime.h>

#include <ostream>


namespace base {

TimeDelta TimeDelta::FromDays(int days) {
  return TimeDelta(days * Time::kMicrosecondsPerDay);
}


TimeDelta TimeDelta::FromHours(int hours) {
  return TimeDelta(hours * Time::kMicrosecondsPerHour);
}


TimeDelta TimeDelta::FromMinutes(int minutes) {
  return TimeDelta(minutes * Time::kMicrosecondsPerMinute);
}


TimeDelta TimeDelta::FromSeconds(int64_t seconds) {
  return TimeDelta(seconds * Time::kMicrosecondsPerSecond);
}


TimeDelta TimeDelta::FromMilliseconds(int64_t milliseconds) {
  return TimeDelta(milliseconds * Time::kMicrosecondsPerMillisecond);
}


TimeDelta TimeDelta::FromNanoseconds(int64_t nanoseconds) {
  return TimeDelta(nanoseconds / Time::kNanosecondsPerMicrosecond);
}


int TimeDelta::InDays() const {
  return static_cast<int>(delta_ / Time::kMicrosecondsPerDay);
}


int TimeDelta::InHours() const {
  return static_cast<int>(delta_ / Time::kMicrosecondsPerHour);
}


int TimeDelta::InMinutes() const {
  return static_cast<int>(delta_ / Time::kMicrosecondsPerMinute);
}


double TimeDelta::InSecondsF() const {
  return static_cast<double>(delta_) / Time::kMicrosecondsPerSecond;
}

int64_t TimeDelta::InSeconds() const {
  return delta_ / Time::kMicrosecondsPerSecond;
}


double TimeDelta::InMillisecondsF() const {
  return static_cast<double>(delta_) / Time::kMicrosecondsPerMillisecond;
}


int64_t TimeDelta::InMilliseconds() const {
  return delta_ / Time::kMicrosecondsPerMillisecond;
}


int64_t TimeDelta::InNanoseconds() const {
  return delta_ * Time::kNanosecondsPerMicrosecond;
}

TimeDelta TimeDelta::FromTimespec(struct timespec ts) {
  DCHECK_GE(ts.tv_nsec, 0);
  DCHECK_LT(ts.tv_nsec,
            static_cast<long>(Time::kNanosecondsPerSecond));  // NOLINT
  return TimeDelta(ts.tv_sec * Time::kMicrosecondsPerSecond +
                    ts.tv_nsec / Time::kNanosecondsPerMicrosecond);
}


struct timespec TimeDelta::ToTimespec() const {
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(delta_ / Time::kMicrosecondsPerSecond);
  ts.tv_nsec = (delta_ % Time::kMicrosecondsPerSecond) *
      Time::kNanosecondsPerMicrosecond;
  return ts;
}

/////////////////////////////////////////////

std::string Time::Format(const std::string format) const {
  char buf[64] = {0};
  struct timeval tv = ToTimeval();  
  //struct tm* tmp = gmtime(&(tv.tv_sec));
  struct tm* tmp = std::localtime(&(tv.tv_sec));

  DCHECK(tmp != nullptr);  
  int n = strftime(buf, 64, format.c_str(), tmp);
  DCHECK(n > 0 && n < 64);
  return std::string(buf, n);
}

Time Time::Now() {
  struct timeval tv;
  int result = gettimeofday(&tv, nullptr);
  DCHECK_EQ(0, result);
  return FromTimeval(tv);
}

Time Time::NowFromSystemTime() {
  return Now();
}


Time Time::FromTimespec(struct timespec ts) {
  DCHECK(ts.tv_nsec >= 0);
  DCHECK(ts.tv_nsec < static_cast<long>(kNanosecondsPerSecond));  // NOLINT
  if (ts.tv_nsec == 0 && ts.tv_sec == 0) {
    return Time();
  }
  if (ts.tv_nsec == static_cast<long>(kNanosecondsPerSecond - 1) &&  // NOLINT
      ts.tv_sec == std::numeric_limits<time_t>::max()) {
    return Max();
  }
  return Time(ts.tv_sec * kMicrosecondsPerSecond +
              ts.tv_nsec / kNanosecondsPerMicrosecond);
}

struct timespec Time::ToTimespec() const {
  struct timespec ts;
  if (IsNull()) {
    ts.tv_sec = 0;
    ts.tv_nsec = 0;
    return ts;
  }
  if (IsMax()) {
    ts.tv_sec = std::numeric_limits<time_t>::max();
    ts.tv_nsec = static_cast<long>(kNanosecondsPerSecond - 1);  // NOLINT
    return ts;
  }
  ts.tv_sec = static_cast<time_t>(us_ / kMicrosecondsPerSecond);
  ts.tv_nsec = (us_ % kMicrosecondsPerSecond) * kNanosecondsPerMicrosecond;
  return ts;
}

Time Time::FromTimeval(struct timeval tv) {
  DCHECK(tv.tv_usec >= 0);
  DCHECK(tv.tv_usec < static_cast<suseconds_t>(kMicrosecondsPerSecond));
  if (tv.tv_usec == 0 && tv.tv_sec == 0) {
    return Time();
  }
  if (tv.tv_usec == static_cast<suseconds_t>(kMicrosecondsPerSecond - 1) &&
            tv.tv_sec == std::numeric_limits<time_t>::max()) {
    return Max();
  }
  return Time(tv.tv_sec * kMicrosecondsPerSecond + tv.tv_usec);
}

struct timeval Time::ToTimeval() const {
  struct timeval tv;
  if (IsNull()) {
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    return tv;
  }
  if (IsMax()) {
    tv.tv_sec = std::numeric_limits<time_t>::max();
    tv.tv_usec = static_cast<suseconds_t>(kMicrosecondsPerSecond - 1);
    return tv;
  }
  tv.tv_sec = static_cast<time_t>(us_ / kMicrosecondsPerSecond);
  tv.tv_usec = us_ % kMicrosecondsPerSecond;
  return tv;
}

////////////////////////////
TimeTicks TimeTicks::Now() {
  return HighResolutionNow();
}

TimeTicks TimeTicks::HighResolutionNow() {
  int64_t ticks;
  struct timespec ts;
  int result = clock_gettime(CLOCK_MONOTONIC, &ts);
  DCHECK_EQ(0, result);
  //USE(result);
  ticks = (ts.tv_sec * Time::kMicrosecondsPerSecond +
           ts.tv_nsec / Time::kNanosecondsPerMicrosecond);
  return TimeTicks(ticks + 1);
}

bool TimeTicks::IsHighResolutionClockWorking() {
  return true;
}

std::ostream& operator<<(std::ostream& os, const Time& time) {
  return os << time.Format("%Y-%m-%d %H:%M:%S");
}


///////////////
typedef time_t SysTime;

SysTime SysTimeFromTimeStruct(struct tm* timestruct, bool is_local) {
  if (is_local)
    return mktime(timestruct);
  else
    return timegm(timestruct);
}
  
void SysTimeToTimeStruct(SysTime t, struct tm* timestruct, bool is_local) {
  if (is_local)
    localtime_r(&t, timestruct);
  else
    gmtime_r(&t, timestruct);
}

static const int64_t kWindowsEpochDeltaSeconds = INT64_C(11644473600);

const int64_t Time::kWindowsEpochDeltaMicroseconds =
    kWindowsEpochDeltaSeconds * Time::kMicrosecondsPerSecond;

const int64_t Time::kTimeTToMicrosecondsOffset = kWindowsEpochDeltaMicroseconds;


void Time::Explode(bool is_local, Exploded* exploded) const {
  int64_t microseconds = us_ - kWindowsEpochDeltaMicroseconds;
  int64_t milliseconds;  // Milliseconds since epoch.
  SysTime seconds;  // Seconds since epoch.
  int millisecond;  // Exploded millisecond value (0-999).
  if (microseconds >= 0) {
    milliseconds = microseconds / kMicrosecondsPerMillisecond;
    seconds = milliseconds / kMillisecondsPerSecond;
    millisecond = milliseconds % kMillisecondsPerSecond;
  } else {
    milliseconds = (microseconds - kMicrosecondsPerMillisecond + 1) /
      kMicrosecondsPerMillisecond;
    seconds = (milliseconds - kMillisecondsPerSecond + 1) /
      kMillisecondsPerSecond;
    millisecond = milliseconds % kMillisecondsPerSecond;
    if (millisecond < 0)
      millisecond += kMillisecondsPerSecond;
   }

  struct tm timestruct;
  SysTimeToTimeStruct(seconds, &timestruct, is_local);

  exploded->year         = timestruct.tm_year + 1900;
  exploded->month        = timestruct.tm_mon + 1;
  exploded->day_of_week  = timestruct.tm_wday;
  exploded->day_of_month = timestruct.tm_mday;
  exploded->hour         = timestruct.tm_hour;
  exploded->minute       = timestruct.tm_min;
  exploded->second       = timestruct.tm_sec;
  exploded->millisecond  = millisecond;
}

bool Time::FromExploded(bool is_local, 
		        const Exploded& exploded, Time* time)   {
  struct tm timestruct;
  timestruct.tm_sec    = exploded.second;
  timestruct.tm_min    = exploded.minute;
  timestruct.tm_hour   = exploded.hour;
  timestruct.tm_mday   = exploded.day_of_month;
  timestruct.tm_mon    = exploded.month - 1;
  timestruct.tm_year   = exploded.year - 1900;
  timestruct.tm_wday   = exploded.day_of_week;  // mktime/timegm ignore this
  timestruct.tm_yday   = 0;     // mktime/timegm ignore this
  timestruct.tm_isdst  = -1;    // attempt to figure it out
  timestruct.tm_gmtoff = 0;     // not a POSIX field, so mktime/timegm ignore
  timestruct.tm_zone   = NULL;  // not a POSIX field, so mktime/timegm ignore
 
  int64_t milliseconds;
  SysTime seconds;

  struct tm timestruct0 = timestruct;
  seconds = SysTimeFromTimeStruct(&timestruct, is_local);
  if (seconds == -1) {
    timestruct = timestruct0;
    timestruct.tm_isdst = 0;
    int64_t seconds_isdst0 = SysTimeFromTimeStruct(&timestruct, is_local);

    timestruct = timestruct0;
    timestruct.tm_isdst = 1;
    int64_t seconds_isdst1 = SysTimeFromTimeStruct(&timestruct, is_local);
						    
    if (seconds_isdst0 < 0)
      seconds = seconds_isdst1;
    else if (seconds_isdst1 < 0)
      seconds = seconds_isdst0;
    else
      seconds = std::min(seconds_isdst0, seconds_isdst1);
  }
        
  if (seconds == -1 &&
      (exploded.year < 1969 || exploded.year > 1970)) {

    const int64_t min_seconds = (sizeof(SysTime) < sizeof(int64_t))
    ? std::numeric_limits<SysTime>::min()
    : std::numeric_limits<int32_t>::min();
    const int64_t max_seconds = (sizeof(SysTime) < sizeof(int64_t))
    ? std::numeric_limits<SysTime>::max()
    : std::numeric_limits<int32_t>::max();
    if (exploded.year < 1969) {
      milliseconds = min_seconds * kMillisecondsPerSecond;
    } else {
      milliseconds = max_seconds * kMillisecondsPerSecond;
      milliseconds += (kMillisecondsPerSecond - 1);
    }
  } else {
    milliseconds = seconds * kMillisecondsPerSecond + exploded.millisecond;
  }
  base::Time converted_time =
    Time((milliseconds * kMicrosecondsPerMillisecond) +
    kWindowsEpochDeltaMicroseconds);
	     
  base::Time::Exploded to_exploded;
  if (!is_local)
    ; //converted_time.UTCExplode(&to_exploded);
  else
    converted_time.LocalExplode(&to_exploded);
		         
  if (ExplodedMostlyEquals(to_exploded, exploded)) {
    *time = converted_time;
    return true;
  }
			     
  *time = Time(0);
  return false;
}

bool Time::FromStringInternal(const char* time_string,
                              bool is_local,
                              Time* parsed_time) {
  DCHECK((time_string != NULL) && (parsed_time != NULL));

  if (time_string[0] == '\0')
    return false;

  PRTime result_time = 0;
  PRStatus result = PR_ParseTimeString(time_string,
                                       is_local ? PR_FALSE : PR_TRUE,
                                       &result_time);
  if (PR_SUCCESS != result)
    return false;

  result_time += kTimeTToMicrosecondsOffset;
  *parsed_time = Time(result_time);
  return true;
}

bool Time::ExplodedMostlyEquals(const Exploded& lhs, const Exploded& rhs) {
  return lhs.year == rhs.year && lhs.month == rhs.month &&
	 lhs.day_of_month == rhs.day_of_month && lhs.hour == rhs.hour &&
	 lhs.minute == rhs.minute && lhs.second == rhs.second &&
	 lhs.millisecond == rhs.millisecond;
}

 Time Time::FromTimeT(time_t tt) {
  if (tt == 0)
    return Time();  // Preserve 0 so we can tell it doesn't exist.
  if (tt == std::numeric_limits<time_t>::max())
    return Max();
  return Time(kTimeTToMicrosecondsOffset) + TimeDelta::FromSeconds(tt);
}

time_t Time::ToTimeT() const {
  if (IsNull())
    return 0;  // Preserve 0 so we can tell it doesn't exist.
  if (IsMax()) {
    return std::numeric_limits<time_t>::max();
  }
  if (std::numeric_limits<int64_t>::max() - kTimeTToMicrosecondsOffset <= us_  ) {
    DLOG(WARNING) << "Overflow when converting base::Time with internal " <<
    "value " << us_ << " to time_t.";
    return std::numeric_limits<time_t>::max();
 }
  return (us_ - kTimeTToMicrosecondsOffset) / kMicrosecondsPerSecond;
}


} // namespace base
