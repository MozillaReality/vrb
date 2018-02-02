#ifndef VRB_UPDATABLE_DOT_H
#define VRB_UPDATABLE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class Updatable {
public:
  virtual void UpdateResource(Context& aContext) = 0;
protected:
  struct State;
  Updatable(State& aState, ContextWeak& aContext);
  Updatable(State& aState);
  virtual ~Updatable();
private:
  State& m;
  Updatable() = delete;
  VRB_NO_DEFAULTS(Updatable)
};

} // namespace vrb

#endif //  VRB_UPDATABLE_DOT_H
