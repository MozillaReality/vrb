/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_GROUP_STATE_DOT_H
#define VRB_GROUP_STATE_DOT_H

#include "vrb/Forward.h"
#include "vrb/private/NodeState.h"
#include <vector>

namespace vrb {

struct Group::State : public Node::State {
  std::vector<NodePtr> children;
  std::vector<LightPtr> lights;
  GroupWeak self;
  bool Contains(const Node& aNode);
  bool Contains(const Light& aLight);
  virtual bool IsEnabled(const Node&) { return true; }
  virtual void Clear() { children.clear(); }
};

}

#endif // VRB_GROUP_STATE_DOT_H
