/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_CREATION_CONTEXT_DOT_H
#define VRB_CREATION_CONTEXT_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class CreationContext {
public:
  static CreationContextPtr Create(RenderContextPtr& aContext);
  void BindToThread();
  void Synchronize();

  void RegisterContextSynchronizerObserver(ContextSynchronizerObserverPtr& aObserver);
  void ReleaseContextSynchronizerObserver(ContextSynchronizerObserverPtr& aObserver);
  void SetFileReader(FileReaderPtr aFileReader);
  FileReaderPtr GetFileReader();
  TextureGLPtr LoadTexture(const std::string& TextureName, const bool aUseCache = true);
  void AddResourceGL(ResourceGL* aResource);
  void AddUpdatable(Updatable* aUpdatable);
protected:
  struct State;
  CreationContext(State& aState);
  ~CreationContext();

private:
  State& m;
  CreationContext() = delete;
  VRB_NO_DEFAULTS(CreationContext)
};

}

#endif // VRB_CREATION_CONTEXT_DOT_H
