/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_SURFACE_TEXTURE_FACTORY_DOT_H
#define VRB_SURFACE_TEXTURE_FACTORY_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include "vrb/ResourceGL.h"
#include "vrb/Updatable.h"

#include "vrb/gl.h"
#include <jni.h>
#include <string>

namespace vrb {

class SurfaceTextureObserver {
public:
  virtual void SurfaceTextureCreated(const std::string& aName, GLuint aHandle, jobject aSurfaceTexture) = 0;
  virtual void SurfaceTextureHandleUpdated(const std::string aName, GLuint aHandle) = 0;
  virtual void SurfaceTextureDestroyed(const std::string& aName) = 0;
  virtual void SurfaceTextureCreationError(const std::string& aName, const std::string& aReason) = 0;
protected:
  SurfaceTextureObserver() {}
  virtual ~SurfaceTextureObserver() {}
private:
  VRB_NO_DEFAULTS(SurfaceTextureObserver)
};

class SurfaceTextureFactory : protected Updatable, protected ResourceGL {
public:
  static SurfaceTextureFactoryPtr Create(CreationContextPtr& aContext);
  void InitializeJava(JNIEnv* aEnv);
  void ShutdownJava();

  void CreateSurfaceTexture(const std::string& aName, SurfaceTextureObserverPtr aObserver);
  void DestroySurfaceTexture(const std::string& aName);
  void AddGlobalObserver(SurfaceTextureObserverPtr aObserver);
  void RemoveGlobalObserver(const SurfaceTextureObserver& aObserver);

  jobject LookupSurfaceTexture(const std::string& aName);

protected:
  struct State;
  SurfaceTextureFactory(State& aState, CreationContextPtr& aContext);
  ~SurfaceTextureFactory();

  // Updatable interface
  void UpdateResource(RenderContext& aContext) override;

  // ResourceGL interface
  void InitializeGL() override;
  void ShutdownGL() override;

private:
  State& m;
  SurfaceTextureFactory() = delete;
  VRB_NO_DEFAULTS(SurfaceTextureFactory)
};

} // namespace vrb

#endif //  VRB_SURFACE_TEXTURE_FACTORY_DOT_H
