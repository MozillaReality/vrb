/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_NODE_STATE_DOT_H
#define VRB_NODE_STATE_DOT_H

#include "vrb/Node.h"
#include "vrb/Group.h"

#include <string>
#include <vector>

namespace vrb {
struct Node::State {
  std::string name;
  std::vector<GroupPtr> parents;
};

}
#endif // VRB_NODE_STATE_DOT_H
