#include "vrb/CullVisitor.h"
#include "vrb/private/CullVisitorState.h"

#include "vrb/ConcreteClass.h"

namespace vrb {

void
CullVisitor::State::Reset() {
  TransformNode* current = transformList;
  while (current) {
    TransformNode* tmp = current;
    current = current->prev;
    delete tmp;
  }
}

CullVisitorPtr
CullVisitor::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<CullVisitor, CullVisitor::State> >(aContext);
}

const Matrix&
CullVisitor::GetTransform() const {
  if (!m.transformList) {
    return m.identity;
  }

  return m.transformList->transform;
}

void
CullVisitor::PushTransform(const Matrix& aTransform) {
  State::TransformNode* node = new State::TransformNode;
  if (m.transformList) {
    node->transform = m.transformList->transform.PreMultiply(aTransform);
  } else {
    node->transform = aTransform;
  }
  node->prev = m.transformList;
  m.transformList = node;
}

void
CullVisitor::PopTransform() {
  State::TransformNode* node = m.transformList;
  if (node) {
    m.transformList = node->prev;
    delete node;
  }
}

CullVisitor::CullVisitor(State& aState, ContextWeak& aContext) : m(aState) {}
CullVisitor::~CullVisitor() {}

} // namespace vrb


