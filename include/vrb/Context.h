/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_CONTEXT_DOT_H
#define VRB_CONTEXT_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#if defined(ANDROID)
#  include <jni.h>
#endif // defined(ANDROID)

namespace vrb {

class Context {
public:
  static ContextPtr Create();

#if defined(ANDROID)
  void InitializeJava(JNIEnv* aEnv, jobject& aActivity, jobject& aAssetManager);
  void ShutdownJava();
#endif // defined(ANDROID)
  bool InitializeGL();
  void ShutdownGL();
  void Update();

  void AddResourceGL(ResourceGL* aResource);
  void AddUpdatable(Updatable* aUpdatable);
  FileReaderPtr GetFileReader();
  TextureCachePtr GetTextureCache();
  GLExtensionsPtr GetGLExtensions() const;
#if defined(ANDROID)
  SurfaceTextureFactoryPtr GetSurfaceTextureFactory();
#endif // defined(ANDROID)

protected:
  struct State;
  Context(State& aState);
  ~Context();

private:
  State& m;
  Context() = delete;
  VRB_NO_DEFAULTS(Context)
};

}

#endif // VRB_CONTEXT_DOT_H
