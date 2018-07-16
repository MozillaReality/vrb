/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/Node.h"
#include "vrb/private/NodeState.h"
#include "vrb/Logger.h"

namespace vrb {

const std::string&
Node::GetName() const { return m.name; }

void
Node::SetName(const std::string& aName) { m.name = aName; }

void
Node::GetParents(std::vector<GroupPtr>& aParents) const {
  for (GroupWeak& weak: m.parents) {
    if (GroupPtr parent = weak.lock()) {
      aParents.push_back(std::move(parent));
    } else {
      VRB_WARN("Deleted weak parent found in parent list in Node::GetParents Node: %s", m.name.c_str());
    }
  }
}

void
Node::RemoveFromParents() {
  for (GroupWeak& weak: m.parents) {
    if (GroupPtr parent = weak.lock()) {
      parent->RemoveNode(*this);
    } else {
      VRB_WARN("Deleted weak parent found in parent list in Node::RemoveFromParents Node: %s", m.name.c_str());
    }
  }
  if (m.parents.size() != 0) {
    VRB_WARN("Node::RemoveFromParents failed to remove all parents for Node: %s", m.name.c_str());
  }
}

Node::Node(State& aState, CreationContextPtr& aContext) : m(aState) {}
Node::~Node() {
  if (m.parents.size() != 0) {
    const char* name = (m.name.size() ? m.name.c_str() : "<unnamed>");
    VRB_WARN("Node: %s destructor called with parent count != 0", name);
  }
}

void
Node::AddToParents(GroupWeak& aParent, Node& aChild) {
  aChild.m.parents.push_back(aParent);
}

void
Node::RemoveFromParents(Group& aParent, Node& aChild) {
  for (auto it = aChild.m.parents.begin(); it != aChild.m.parents.end();) {
    Group* node = it->lock().get();
    if (node == &aParent) {
      it = aChild.m.parents.erase(it);
      return;
    } else if (node == nullptr) {
      it = aChild.m.parents.erase(it);
    } else {
      it++;
    }
  }
}

bool
Node::Traverse(const NodePtr& aRootNode, const TraverseFunction& aTraverseFunction) {
  if (aTraverseFunction(aRootNode, nullptr)) {
    return true;
  }
  return aRootNode->Traverse(std::dynamic_pointer_cast<Group>(aRootNode), aTraverseFunction);
}

bool
Node::Traverse(const GroupPtr& aParent, const TraverseFunction& aTraverseFunction) {
  return false;
}

}
