#include "vrb/Transform.h"
#include "vrb/private/TransformState.h"

#include "vrb/ConcreteClass.h"
#include "vrb/CullVisitor.h"

#include <memory>

namespace vrb {

TransformPtr
Transform::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<Transform, Transform::State> > (aContext);
}

void
Transform::Cull(CullVisitorPtr& aVisitor, DrawableListPtr& aDrawables) {
  aVisitor->PushTransform(m.transform);
  Group::Cull(aVisitor, aDrawables);
  aVisitor->PopTransform();
}

const Matrix&
Transform::GetTransform() const {
  return m.transform;
}

void
Transform::SetTransform(const Matrix& aTransform) {
  m.transform = aTransform;
}

Transform::Transform(State& aState, ContextWeak& aContext) : Group(aState, aContext), m(aState) {}
Transform::~Transform() {}

}
