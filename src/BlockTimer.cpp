#include "vrb/BlockTimer.h"

#include <ctime>
#include <vrb/Logger.h>

namespace {
const double kNanosecondsToSeconds = 1.0e9;
}

namespace vrb {

BlockTimer::BlockTimer(const char* aFileName, const char* aFunctionName, const int aLineNumber, const double aMaxTime, BlockTimerCallback aCallback) :
    mFileName(aFileName ? aFileName : "Unknown File"),
    mFunctionName(aFunctionName ? aFunctionName : "Unknown Function"),
    mLineNumber(aLineNumber),
    mMaxTime(aMaxTime),
    mStartTime(0.0),
    mCallback(aCallback) {
  timespec spec = {};
  if (clock_gettime(CLOCK_MONOTONIC, &spec) == 0) {
    mStartTime = (double)spec.tv_sec + (spec.tv_nsec / kNanosecondsToSeconds);
  }
}

BlockTimer::~BlockTimer() {
  timespec spec = {};
  if (clock_gettime(CLOCK_MONOTONIC, &spec) == 0) {
    const double value = ((double)spec.tv_sec + (spec.tv_nsec / kNanosecondsToSeconds)) - mStartTime;
    if (mMaxTime < value) {
      VRB_ERROR("*** Block overrun: %s:%s[%d] %f(sec) > %f(sec)", mFileName, mFunctionName, mLineNumber, value, mMaxTime);
      if (mCallback) {
        mCallback();
      }
    }
  }
}

} // vrb
