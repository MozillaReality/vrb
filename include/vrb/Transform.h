#ifndef VRB_TRANSFORM_DOT_H
#define VRB_TRANSFORM_DOT_H

#include "vrb/Forward.h"
#include "vrb/Group.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class Transform : public Group {
public:
  static TransformPtr Create(ContextWeak& aContext);
  // Node interface
  void Cull(CullVisitorPtr& aVisitor, DrawableListPtr& aDrawables) override;
  // Transform interface
  const Matrix& GetTransform() const;
  void SetTransform(const Matrix& aTransform);
protected:
  struct State;
  Transform(State& aState, ContextWeak& aContext);
  ~Transform();
private:
  State& m;
  Transform() = delete;
  VRB_NO_DEFAULTS(Transform)
};

}

#endif //  VRB_TRANSFORM_DOT_H
