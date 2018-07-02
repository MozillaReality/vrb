/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/Toggle.h"
#include "vrb/private/GroupState.h"
#include "vrb/ConcreteClass.h"

#include <unordered_set>

namespace vrb {

struct Toggle::State : public Group::State {
  std::unordered_set<const Node*> toggledOff;
  bool IsEnabled(const Node& aNode) override { return toggledOff.count(&aNode) == 0; }
  void Clear() override { toggledOff.clear(); Group::State::Clear(); }
};

TogglePtr
Toggle::Create(CreationContextPtr& aContext) {
  TogglePtr toggle = std::make_shared<ConcreteClass<Toggle, Toggle::State> >(aContext);
  toggle->m.self = toggle;
  return toggle;
}

// Group interface
void
Toggle::RemoveNode(Node& aNode) {
  m.toggledOff.erase(&aNode);
  Super::RemoveNode(aNode);
}

// Toggle interface
void
Toggle::ToggleAll(const bool aEnabled) {
  if (aEnabled) {
    m.toggledOff.clear();
    return;
  }
  for (const NodePtr& node: m.children) {
    m.toggledOff.insert(node.get());
  }
}

bool
Toggle::IsEnabled(const Node& aNode) {
  return m.IsEnabled(aNode);
}

void
Toggle::ToggleChild(const Node& aNode, const bool aEnabled) {
  if (!m.Contains(aNode)) {
    return;
  }
  if (aEnabled) {
    m.toggledOff.erase(&aNode);
    return;
  }

  m.toggledOff.insert(&aNode);
}

Toggle::Toggle(State& aState, CreationContextPtr& aContext) : Group(aState, aContext), m(aState) {}
Toggle::~Toggle() {}

} // namespace vrb
