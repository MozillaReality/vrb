#ifndef VRB_CONCRETE_CLASS_DOT_H
#define VRB_CONCRETE_CLASS_DOT_H

#include "vrb/Forward.h"

namespace vrb {

template<class Base, class State>
class ConcreteClass : public Base {
  State mConcreteClassInternalState;
public:
  ConcreteClass() : Base(mConcreteClassInternalState) {}
  ConcreteClass(ContextWeak& aContext) : Base(mConcreteClassInternalState, aContext) {}
};

} // namespace vrb

#endif // VRB_CONCRETE_CLASS_DOT_H
