#include "vrb/Drawable.h"
#include "vrb/private/DrawableState.h"

namespace vrb {

DrawablePtr
Drawable::CreateDrawablePtr() {
  return shared_from_this();
}

Drawable::Drawable(State& aState, ContextWeak& aContext) : m(aState) {}
Drawable::~Drawable() {}

}
