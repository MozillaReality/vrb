/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/Group.h"
#include "vrb/private/GroupState.h"

#include "vrb/ConcreteClass.h"
#include "vrb/DrawableList.h"
#include "vrb/Light.h"

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
  return std::make_shared<ConcreteClass<Group, Group::State> >(aContext);
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
    m.children.push_back(std::move(aNode));
  }
}

void
Group::RemoveNode(const Node& aNode) {
  for (auto it = m.children.begin(); it != m.children.end(); it++) {
    if (it->get() == &aNode) {
      m.children.erase(it);
      return;
    }
  }
}

Group::Group(State& aState, ContextWeak& aContext) : Node(aState, aContext), m(aState) {}
Group::~Group() {}

}
