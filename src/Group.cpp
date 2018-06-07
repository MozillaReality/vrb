/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/Group.h"
#include "vrb/private/GroupState.h"

#include "vrb/ConcreteClass.h"
#include "vrb/DrawableList.h"
#include "vrb/Light.h"
#include <algorithm>
#include "vrb/Logger.h"

#include <memory>

namespace vrb {

bool
Group::State::Contains(const Node& aNode) {
  for (NodePtr& node: children) {
    if (node.get() == &aNode) {
      return true;
    }
  }
  return false;
}

bool
Group::State::Contains(const Light& aLight) {
  for (LightPtr& light: lights) {
    if (light.get() == &aLight) {
      return true;
    }
  }
  return false;
}

GroupPtr
Group::Create(ContextWeak& aContext) {
  GroupPtr group = std::make_shared<ConcreteClass<Group, Group::State> >(aContext);
  group->m.self = group;
  return group;
}

void
Group::Cull(CullVisitor& aVisitor, DrawableList& aDrawables) {
  for (LightPtr& light: m.lights) {
    aDrawables.PushLight(*light);
  }
  for (NodePtr& node: m.children) {
    if (m.IsEnabled(*node)) {
      node->Cull(aVisitor, aDrawables);
    }
  }
  aDrawables.PopLights(m.lights.size());
}

void
Group::AddLight(LightPtr aLight) {
  if (!m.Contains(*aLight)) {
    m.lights.push_back(std::move(aLight));
  }
}

void
Group::RemoveLight(const Light& aLight) {
  for (auto it = m.lights.begin(); it != m.lights.end(); it++) {
    if (it->get() == &aLight) {
      m.lights.erase(it);
      return;
    }
  }
}

void
Group::AddNode(NodePtr aNode) {
  if (!m.Contains(*aNode)) {
    AddToParents(m.self, *aNode);
    m.children.push_back(std::move(aNode));
  }
}

void
Group::RemoveNode(Node& aNode) {
  for (auto childIt = m.children.begin(); childIt != m.children.end(); childIt++) {
    if (childIt->get() == &aNode) {
      m.children.erase(childIt);
      RemoveFromParents(m.self, aNode);
      return;
    }
  }
}

void
Group::SortNodes(const std::function<bool(const vrb::NodePtr&, const vrb::NodePtr&)>& aFunction) {
  std::sort(m.children.begin(), m.children.end(), aFunction);
}

bool
Group::Traverse(const GroupPtr& aParent, const Node::TraverseFunction& aTraverseFunction) {
  for (NodePtr& child: m.children) {
    if (aTraverseFunction(child, aParent)) {
      return true;
    }
  }

  return false;
}

Group::Group(State& aState, ContextWeak& aContext) : Node(aState, aContext), m(aState) {}
Group::~Group() {
  for (NodePtr& child: m.children) {
    RemoveFromParents(m.self, *child);
  }
}

}
