/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_CONTEXT_SYNCHRONIZER_DOT_H
#define VRB_CONTEXT_SYNCHRONIZER_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class ContextSynchronizerObserver {
public:
  virtual void ContextsSynchronized(RenderContextPtr& aRenderContext) = 0;
};

class ContextSynchronizer {
public:
  static ContextSynchronizerPtr Create(RenderContextPtr& aContext);
  void BindToThread();
  void RegisterObserver(ContextSynchronizerObserverPtr& aObserver);
  void ReleaseObserver(ContextSynchronizerObserverPtr& aObserver);
  void AdoptLists(
      ResourceGLList& aUninitializedResources,
      ResourceGLList& aResources,
      UpdatableList& aUpdatables);
  void Signal(bool& aIsActive);
  void Release();
protected:
  struct State;
  ContextSynchronizer(State& aState);
  ~ContextSynchronizer();
private:
  State& m;
  ContextSynchronizer() = delete;
  VRB_NO_DEFAULTS(ContextSynchronizer)
};

}

#endif // VRB_CONTEXT_SYNCHRONIZER_DOT_H
