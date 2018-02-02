#include "vrb/Updatable.h"

#include "vrb/Context.h"

namespace vrb {

Updatable::Updatable(State& aState, ContextWeak& aContext) : m(aState) {
  aContext.lock()->AddUpdatable(this);
}
Updatable::Updatable(State& aState) : m(aState) {}

Updatable::~Updatable() {}

} // namespace vrb
