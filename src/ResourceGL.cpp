#include "vrb/ResourceGL.h"

#include "vrb/Context.h"

namespace vrb {

ResourceGL::ResourceGL(State& aState, ContextWeak& aContext) : m(aState) {
  aContext.lock()->AddResourceGL(this);
}
ResourceGL::ResourceGL(State& aState) : m(aState) {}

ResourceGL::~ResourceGL() {}

} // namespace vrb
