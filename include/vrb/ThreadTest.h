#ifndef VRB_THREAD_TEST_DOT_H
#define VRB_THREAD_TEST_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class ThreadTest {
public:
  static ThreadTestPtr Create();
  bool IsOnInitializationThread();
protected:
  struct State;
  ThreadTest(State& aState);
  ~ThreadTest() = default;
private:
  State& m;
  ThreadTest() = delete;
  VRB_NO_DEFAULTS(ThreadTest)
};

} // vrb
#endif // VRB_THREAD_TEST_DOT_H
