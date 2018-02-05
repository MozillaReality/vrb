/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/Drawable.h"
#include "vrb/private/DrawableState.h"

namespace vrb {

DrawablePtr
Drawable::CreateDrawablePtr() {
  return shared_from_this();
}

Drawable::Drawable(State& aState, ContextWeak& aContext) : m(aState) {}
Drawable::~Drawable() {}

}
