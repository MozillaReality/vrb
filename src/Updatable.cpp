/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/Updatable.h"

#include "vrb/Context.h"

namespace vrb {

Updatable::Updatable(State& aState, ContextWeak& aContext) : m(aState) {
  aContext.lock()->AddUpdatable(this);
}
Updatable::Updatable(State& aState) : m(aState) {}

Updatable::~Updatable() {}

} // namespace vrb
