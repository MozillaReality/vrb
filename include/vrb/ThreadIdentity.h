#ifndef VRB_THREAD_IDENTITY_DOT_H
#define VRB_THREAD_IDENTITY_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class ThreadIdentity {
public:
  static ThreadIdentityPtr Create();
  bool IsOnInitializationThread() const;
  bool IsSameThread(const ThreadIdentity& Identity) const;
protected:
  struct State;
  ThreadIdentity(State& aState);
  ~ThreadIdentity() = default;
private:
  State& m;
  ThreadIdentity() = delete;
  VRB_NO_DEFAULTS(ThreadIdentity)
};

} // vrb
#endif // VRB_THREAD_IDENTITY_DOT_H
