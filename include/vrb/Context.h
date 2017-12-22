#ifndef VRB_CONTEXT_DOT_H
#define VRB_CONTEXT_DOT_H

#include "vrb/Forward.h"

namespace vrb {

class Context {
public:
  static ContextPtr Create();

  void Init();
  void Shutdown();

  TextureCachePtr& GetTextureCache();

protected:
  Context();
  ~Context();

  struct State;
  State* m;

private:
  Context(const Context&) = delete;
  Context& operator=(const Context&) = delete;
};

}

#endif // VRB_CONTEXT_DOT_H
