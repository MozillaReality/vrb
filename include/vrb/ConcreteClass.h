#ifndef VRB_CONCRETE_CLASS_DOT_H
#define VRB_CONCRETE_CLASS_DOT_H

#include "vrb/Forward.h"

namespace vrb {

template<class Base, class State>
class ConcreteClass : private State, public Base {
public:
  ConcreteClass() : Base(*(State*)this) {}
  ConcreteClass(ContextWeak& aContext) : Base(*(State*)this, aContext) {}
};

} // namespace vrb

#endif // VRB_CONCRETE_CLASS_DOT_H
