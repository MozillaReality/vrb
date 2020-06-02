/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_CONCRETE_CLASS_DOT_H
#define VRB_CONCRETE_CLASS_DOT_H

#include "vrb/Forward.h"

// VRB_COUNT_OBJECTS enables leak detection.
//#define VRB_COUNT_OBJECTS
#include "vrb/ObjectCounter.h"

namespace vrb {

template<class Base, class State>
class ConcreteClass : private State, public Base {
public:
  ConcreteClass() : Base(*(State*)this) { VRB_ADD_TO_OBJECT_COUNT; }
  ConcreteClass(CreationContextPtr& aContext) : Base(*(State*)this, aContext) { VRB_ADD_TO_OBJECT_COUNT; }
  ConcreteClass(RenderContextPtr& aContext) : Base(*(State*)this, aContext) { VRB_ADD_TO_OBJECT_COUNT; }
  ~ConcreteClass() { VRB_REMOVE_FROM_OBJECT_COUNT; }
};

} // namespace vrb

#endif // VRB_CONCRETE_CLASS_DOT_H
