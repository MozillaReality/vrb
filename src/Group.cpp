#include "vrb/Group.h"
#include "vrb/private/GroupState.h"

#include "vrb/ConcreteClass.h"

#include <memory>

namespace vrb {


bool
Group::State::Contains(const NodePtr& aNode) {
  for (NodePtr& node: children) {
    if (node == aNode) {
      return true;
    }
  }
  return false;
}

GroupPtr
Group::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<Group, Group::State> >(aContext);
}

void
Group::Cull(CullVisitorPtr& aVisitor, DrawableListPtr& aDrawables) {
  for (NodePtr& node: m.children) {
    node->Cull(aVisitor, aDrawables);
  }
}

void
Group::AddNode(const NodePtr aNode) {
  if (!m.Contains(aNode)) {
    m.children.push_back(aNode);
  }
}

void
Group::RemoveNode(const NodePtr aNode) {
  for (auto it = m.children.begin(); it != m.children.end(); it++) {
    if (*it == aNode) {
      m.children.erase(it);
      return;
    }
  }
}

Group::Group(State& aState, ContextWeak& aContext) : Node(aState, aContext), m(aState) {}
Group::~Group() {}

}
