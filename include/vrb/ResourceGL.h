/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_RESOURCE_GL_DOT_H
#define VRB_RESOURCE_GL_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class ResourceGL {
public:
  virtual void InitializeGL(Context& aContext) = 0;
  virtual void ShutdownGL(Context& aContext) = 0;
protected:
  struct State;
  ResourceGL(State& aState, ContextWeak& aContext);
  ResourceGL(State& aState);
  virtual ~ResourceGL();
private:
  State& m;
  ResourceGL() = delete;
  VRB_NO_DEFAULTS(ResourceGL)
};

} // namespace vrb

#endif // VRB_RESOURCE_GL_DOT_H
