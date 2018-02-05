/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/Node.h"
#include "vrb/private/NodeState.h"

namespace vrb {

const std::string&
Node::GetName() const { return m.name; }

void
Node::SetName(const std::string& aName) { m.name = aName; }

Node::Node(State& aState, ContextWeak& aContext) : m(aState) {}
Node::~Node() {}

}
