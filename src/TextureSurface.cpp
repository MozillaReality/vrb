/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/TextureSurface.h"
#include "vrb/private/TextureState.h"
#include "vrb/ConcreteClass.h"

#include "vrb/GLError.h"
#include "vrb/Logger.h"
#include "vrb/RenderContext.h"
#include "vrb/SurfaceTextureFactory.h"

#include "vrb/gl.h"

namespace {

typedef std::weak_ptr<vrb::TextureSurface> SurfaceWeakPtr;
typedef std::weak_ptr<vrb::SurfaceTextureFactory> FactoryWeakPtr;

class LocalObserver : public vrb::SurfaceTextureObserver {
public:
  LocalObserver(SurfaceWeakPtr& aTexture);
  void SurfaceTextureCreated(const std::string& aName, GLuint aHandle, jobject aSurfaceTexture) override;
  void SurfaceTextureHandleUpdated(const std::string aName, GLuint aHandle) override;
  void SurfaceTextureDestroyed(const std::string& aName) override;
  void SurfaceTextureCreationError(const std::string& aName, const std::string& aReason) override;
protected:
  SurfaceWeakPtr mTexture;
};

LocalObserver::LocalObserver(SurfaceWeakPtr& aTexture) : mTexture(aTexture) {}

void
LocalObserver::SurfaceTextureCreated(const std::string& aName, GLuint aHandle, jobject aSurfaceTexture) {
  if (vrb::TextureSurfacePtr texture = mTexture.lock()) {
    texture->SetTextureHandle(aHandle);
  }
}

void
LocalObserver::SurfaceTextureHandleUpdated(const std::string aName, GLuint aHandle) {
  if (vrb::TextureSurfacePtr texture = mTexture.lock()) {
    texture->SetTextureHandle(aHandle);
  }
}

void
LocalObserver::SurfaceTextureDestroyed(const std::string& aName) {
  if (vrb::TextureSurfacePtr texture = mTexture.lock()) {
    texture->SetTextureHandle(0);
  }
}

void
LocalObserver::SurfaceTextureCreationError(const std::string& aName, const std::string& aReason) {
  VRB_LOG("Failed to create SurfaceTexture[%s]: %s", aName.c_str(), aReason.c_str());
}

} // namespace

namespace vrb {

struct TextureSurface::State : public Texture::State {
  FactoryWeakPtr factory;
};

TextureSurfacePtr
TextureSurface::Create(RenderContextPtr& aContext, const std::string& aName) {
  TextureSurfacePtr result = std::make_shared<ConcreteClass<TextureSurface, TextureSurface::State> >(aContext->GetRenderThreadCreationContext());
  result->SetName(aName);
  SurfaceWeakPtr weakPtr = result;
  if (SurfaceTextureFactoryPtr factory = aContext->GetSurfaceTextureFactory()) {
    result->m.factory = factory;
    factory->CreateSurfaceTexture(aName, std::make_shared<LocalObserver>(weakPtr));
  }

  return result;
}

TextureSurface::TextureSurface(State& aState, CreationContextPtr& aContext) : Texture(aState, aContext), m(aState) {
  m.target = GL_TEXTURE_EXTERNAL_OES;
}

TextureSurface::~TextureSurface() {
  if (SurfaceTextureFactoryPtr factory = m.factory.lock()) {
    factory->DestroySurfaceTexture(m.name);
  }
}

void
TextureSurface::SetTextureHandle(const GLuint aHandle) {
  m.texture = aHandle;
}

void
TextureSurface::AboutToBind() {
}

} // namespace vrb
