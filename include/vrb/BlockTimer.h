#ifndef VRB_BLOCK_TIMER_H
#define VRB_BLOCK_TIMER_H

#include <functional>
#include "vrb/MacroUtils.h"

namespace vrb {

typedef std::function<void()> BlockTimerCallback;

#define VRB_BLOCK_TIMER(limit, cb) BlockTimer bt(__FILE__, __FUNCTION__, __LINE__, limit, cb)

#define VRB_BLOCK_TIMER_DEFAULT_TIME 0.025

#define VRB_BLOCK_TIMER_DEFAULT VRB_BLOCK_TIMER(VRB_BLOCK_TIMER_DEFAULT_TIME, nullptr)

#define VRB_BLOCK_TIMER_LIMIT(limit) VRB_BLOCK_TIMER(limit, nullptr)

#define VRB_BLOCK_TIMER_CB(cb) BlockTimer bt(__FILE__, __FUNCTION__, __LINE__, VRB_BLOCK_TIMER_DEFAULT_TIME, cb)

class BlockTimer {
public:
  BlockTimer(const char* aFileName, const char* aFunctionName, const int aLineNumber, const double aMaxTime, BlockTimerCallback aCallback);
  ~BlockTimer();
private:
  const char* mFileName;
  const char* mFunctionName;
  const int mLineNumber;
  const double mMaxTime;
  double mStartTime;
  BlockTimerCallback mCallback;
  BlockTimer() = delete;
  VRB_NO_DEFAULTS(BlockTimer)
  VRB_NO_NEW_DELETE
};

} // vrb
#endif //  VRB_BLOCK_TIMER_H
