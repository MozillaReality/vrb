/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_CONCRETE_CLASS_DOT_H
#define VRB_CONCRETE_CLASS_DOT_H

#include "vrb/Forward.h"

namespace vrb {

template<class Base, class State>
class ConcreteClass : private State, public Base {
public:
  ConcreteClass() : Base(*(State*)this) {}
  ConcreteClass(ContextWeak& aContext) : Base(*(State*)this, aContext) {}
};

} // namespace vrb

#endif // VRB_CONCRETE_CLASS_DOT_H
