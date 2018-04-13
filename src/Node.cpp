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
Node::RemoveFromParents() {
  for (GroupPtr& parent: m.parents) {
    parent->RemoveNode(*this);
  }
  if (m.parents.size() != 0) {
    VRB_LOG("Node::RemoveFromParents failed to remove all parents for Node: %s", m.name.c_str());
  }
}

void
Node::GetParents(std::vector<GroupPtr>& aParents) const {
  aParents = m.parents;
}

Node::Node(State& aState, ContextWeak& aContext) : m(aState) {}
Node::~Node() {
  if (m.parents.size() != 0) {
    VRB_LOG("Node: %s destructor called with parent count != 0", m.name.c_str());
  }
}

}
