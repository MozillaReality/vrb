/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_RENDER_CONTEXT_DOT_H
#define VRB_RENDER_CONTEXT_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#if defined(ANDROID)
#  include <jni.h>
#endif // defined(ANDROID)

namespace vrb {

class RenderContext {
public:
  static RenderContextPtr Create();

#if defined(ANDROID)
  void InitializeJava(JNIEnv* aEnv, jobject& aActivity, jobject& aAssetManager);
  void ShutdownJava();
#endif // defined(ANDROID)
  bool IsOnRenderThread();
  bool InitializeGL();
  void ShutdownGL();
  void Update();
  double GetTimestamp();
  double GetFrameDelta();

  ThreadIdentityPtr& GetRenderThreadIdentity();
  DataCachePtr& GetDataCache();
  TextureCachePtr& GetTextureCache();
  ProgramFactoryPtr& GetProgramFactory();
  CreationContextPtr& GetRenderThreadCreationContext();
  GLExtensionsPtr GetGLExtensions() const;
#if defined(ANDROID)
  SurfaceTextureFactoryPtr GetSurfaceTextureFactory();
#endif // defined(ANDROID)

  // Internal interface
  ResourceGLList& GetUninitializedResourceGLList();
  ResourceGLList& GetResourceGLList();
  UpdatableList& GetUpdatableList();
  void RegisterContextSynchronizer(ContextSynchronizerPtr& aSynchronizer);
protected:
  struct State;
  RenderContext(State& aState);
  ~RenderContext();

private:
  State& m;
  RenderContext() = delete;
  VRB_NO_DEFAULTS(RenderContext)
};

}

#endif // VRB_RENDER_CONTEXT_DOT_H
