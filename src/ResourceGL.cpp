/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/ResourceGL.h"
#include "vrb/private/ResourceGLState.h"

#include "vrb/CreationContext.h"

namespace vrb {

ResourceGL::ResourceGL(State& aState, CreationContextPtr& aContext) : m(aState) {
  aContext->AddResourceGL(this);
}
ResourceGL::ResourceGL(State& aState) : m(aState) {}

ResourceGL::~ResourceGL() {}

} // namespace vrb
