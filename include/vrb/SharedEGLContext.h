/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_SHARED_EGL_CONTEXT_DOT_H
#define VRB_SHARED_EGL_CONTEXT_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class SharedEGLContext {
public:
  static SharedEGLContextPtr Create();
  bool Initialize();
  bool MakeCurrent();
protected:
  struct State;
  SharedEGLContext(State& aState);
  ~SharedEGLContext();
private:
  State& m;
  SharedEGLContext() = delete;
  VRB_NO_DEFAULTS(SharedEGLContext)
};

} // namespace vrb

#endif // VRB_SHARED_EGL_CONTEXT_DOT_H
