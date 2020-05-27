#ifndef VRB_LOADER_GLTF_DOT_H
#define VRB_LOADER_GLTF_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include <memory>

namespace vrb {

class LoaderGLTF;
typedef  std::shared_ptr<LoaderGLTF> LoaderGLTFPtr;

class LoaderGLTF {
public:
  static LoaderGLTFPtr Create(CreationContextPtr& aContext);
protected:
  struct State;
  LoaderGLTF(State& aState, CreationContextPtr& aContext);
  ~LoaderGLTF();
private:
  State& m;
  LoaderGLTF() = delete;
  VRB_NO_DEFAULTS(LoaderGLTF)
};

} // namespace vrb
#endif  // VRB_LOADER_GLTF_DOT_H
