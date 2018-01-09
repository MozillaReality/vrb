#ifndef VRB_RESOURCE_GL_DOT_H
#define VRB_RESOURCE_GL_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class ResourceGL {
public:
  virtual void InitializeGL() = 0;
  virtual void ShutdownGL() = 0;
protected:
  struct State;
  ResourceGL(State& aState, ContextWeak& aContext);
  ResourceGL(State& aState);
  virtual ~ResourceGL();
  void CallAllInitializeGL();
  void CallAllShutdownGL();
private:
  State& m;
  ResourceGL() = delete;
  VRB_NO_DEFAULTS(ResourceGL)
};

} // namespace vrb

#endif // VRB_RESOURCE_GL_DOT_H
