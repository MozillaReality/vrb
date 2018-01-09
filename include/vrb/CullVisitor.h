#ifndef VRB_CULL_VISITOR_DOT_H
#define VRB_CULL_VISITOR_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class CullVisitor {
public:
  static CullVisitorPtr Create(ContextWeak& aContext);
  const Matrix& GetTransform() const;
  void PushTransform(const Matrix& aTransform);
  void PopTransform();

protected:
  struct State;
  CullVisitor(State& aState, ContextWeak& aContext);
  ~CullVisitor();

private:
  State& m;
  CullVisitor() = delete;
  VRB_NO_DEFAULTS(CullVisitor)
};

} // namespace vrb

#endif // VRB_CULL_VISITOR_DOT_H
