#ifndef VRB_BASE_DOT_H
#define VRB_BASE_DOT_H

namespace vrb {

template<class Base, class State>
class Alloc : public Base {
  State mInternalState;
public:
  Alloc() { Base::m = &mInternalState; }
};

} // namespace vrb

#endif // VRB_BASE_DOT_H
