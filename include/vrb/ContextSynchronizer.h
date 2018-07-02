#ifndef VRB_CONTEXT_SYNCHRONIZER_DOT_H
#define VRB_CONTEXT_SYNCHRONIZER_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class ContextSynchronizerObserver {
public:
  virtual void ContextsSynchronized(RenderContextPtr& aRenderContext) = 0;
};

class ContextSynchronizer {
public:
  static ContextSynchronizerPtr Create(RenderContextPtr& aContext);
  void BindToThread();
  void RegisterObserver(ContextSynchronizerObserverPtr& aObserver);
  void ReleaseObserver(ContextSynchronizerObserverPtr& aObserver);
  void AdoptLists(ResourceGLHead& aResourceHead, ResourceGLTail& aResourceTail, UpdatableHead& aUpdatableHead, UpdatableTail& aUpdatableTail);
  void Signal(bool& aIsActive);
  void Release();
protected:
  struct State;
  ContextSynchronizer(State& aState);
  ~ContextSynchronizer();
private:
  State& m;
  ContextSynchronizer() = delete;
  VRB_NO_DEFAULTS(ContextSynchronizer)
};

}

#endif // VRB_CONTEXT_SYNCHRONIZER_DOT_H
