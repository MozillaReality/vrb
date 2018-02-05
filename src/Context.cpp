/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/Context.h"
#include "vrb/private/ResourceGLState.h"
#include "vrb/private/UpdatableState.h"

#include "vrb/ConcreteClass.h"
#if defined(ANDROID)
#include "vrb/FileReaderAndroid.h"
#endif // defined(ANDROID)
#include "vrb/Logger.h"
#include "vrb/ResourceGL.h"
#include "vrb/SurfaceTextureFactory.h"
#include "vrb/TextureCache.h"
#include "vrb/Updatable.h"
#include <EGL/egl.h>

namespace {

class UpdatableAnchor : public vrb::Updatable {
public:
  UpdatableAnchor();
  ~UpdatableAnchor();

  // vrb::Updatable interface
  void UpdateResource(vrb::Context& aContext) override;

  // UpdatableAnchor interface
  void BindTail(UpdatableAnchor& aTail);
protected:
  vrb::Updatable::State m;

};

class UpdatableAnchorTail : public UpdatableAnchor {
public:
  void UpdateResource(vrb::Context&) override {} // noop
  void Prepend(vrb::Updatable* aUpdatable);
};

UpdatableAnchor::UpdatableAnchor() : vrb::Updatable(m) {}
UpdatableAnchor::~UpdatableAnchor() {}

void
UpdatableAnchor::UpdateResource(vrb::Context& aContext) {
  m.CallAllUpdateResources(aContext);
}

void
UpdatableAnchor::BindTail(UpdatableAnchor& aTail) {
  m.nextUpdatable = &aTail;
  aTail.m.prevUpdatable = this;
}

void
UpdatableAnchorTail::Prepend(vrb::Updatable* aUpdatable) {
  m.Prepend(aUpdatable);
}

class ResourceAnchor : public vrb::ResourceGL {
public:
  ResourceAnchor();
  ~ResourceAnchor();

  // vrb::ResourcGL interface
  void InitializeGL(vrb::Context& aContext) override;
  void ShutdownGL(vrb::Context& aContext) override;

  // ResourceAnchor interface
  void BindTail(ResourceAnchor& aTail);
  bool Update(vrb::Context& aContex);
  void Prepend(vrb::ResourceGL* aResource);
  void PrependAndAdoptList(ResourceAnchor& aHead, ResourceAnchor& aTail);
protected:
  vrb::ResourceGL::State m;
};

class ResourceAnchorTail : public ResourceAnchor {
public:
  void InitializeGL(vrb::Context&) override {} // noop
  void ShutdownGL(vrb::Context&) override {} // noop
};

ResourceAnchor::ResourceAnchor() : vrb::ResourceGL(m) {}
ResourceAnchor::~ResourceAnchor() {}

void
ResourceAnchor::BindTail(ResourceAnchor& aTail) {
  m.nextResource = &aTail;
  aTail.m.prevResource = this;
}

void
ResourceAnchor::InitializeGL(vrb::Context& aContext) {
  m.CallAllInitializeGL(aContext);
}

void
ResourceAnchor::ShutdownGL(vrb::Context& aContext) {
  m.CallAllShutdownGL(aContext);
}

bool
ResourceAnchor::Update(vrb::Context& aContext) {
  if (!m.nextResource) {
    return false;
  }
  m.CallAllInitializeGL(aContext);
  return true;
}

void
ResourceAnchor::Prepend(vrb::ResourceGL* aResource) {
  m.Prepend(aResource);
}

void
ResourceAnchor::PrependAndAdoptList(ResourceAnchor& aHead, ResourceAnchor& aTail) {
  m.PrependAndAdoptList(*this, aHead, aTail);
}

}

namespace vrb {

struct Context::State {
  std::weak_ptr<Context> self;
  EGLContext eglContext;
  TextureCachePtr textureCache;
#if defined(ANDROID)
  FileReaderAndroidPtr fileReader;
  SurfaceTextureFactoryPtr surfaceTextureFactory;
#endif // defined(ANDROID)
  UpdatableAnchor updatableHead;
  UpdatableAnchorTail updatableTail;
  ResourceAnchor addedResourcesHead;
  ResourceAnchorTail addedResourcesTail;
  ResourceAnchor resourcesHead;
  ResourceAnchorTail resourcesTail;
  State();
};

Context::State::State() : eglContext(EGL_NO_CONTEXT) {
  updatableHead.BindTail(updatableTail);
  addedResourcesHead.BindTail(addedResourcesTail);
  resourcesHead.BindTail(resourcesTail);
}

ContextPtr
Context::Create() {
  ContextPtr result = std::make_shared<ConcreteClass<Context, Context::State> >();
  result->m.self = result;
  result->m.textureCache = TextureCache::Create(result->m.self);
#if defined(ANDROID)
  result->m.fileReader = FileReaderAndroid::Create(result->m.self);
  result->m.surfaceTextureFactory = SurfaceTextureFactory::Create(result->m.self);
#endif // defined(ANDROID)

  return result;
}

#if defined(ANDROID)
void
Context::InitializeJava(JNIEnv* aEnv, jobject& aAssetManager) {
  if (m.fileReader) { m.fileReader->Init(aEnv, aAssetManager); }
  if (m.surfaceTextureFactory) { m.surfaceTextureFactory->InitializeJava(aEnv); }
}

void
Context::ShutdownJava() {
  if (m.fileReader) { m.fileReader->Shutdown(); }
}
#endif // defined(ANDROID)

bool
Context::InitializeGL() {
  EGLContext current = eglGetCurrentContext();
  if (current == EGL_NO_CONTEXT) {
    VRB_LOG("Unable to initialize VRB context: EGLContext is not valid.");
    m.eglContext = current;
    return false;
  }
  if (current == m.eglContext) {
    VRB_LOG("EGLContext c:%p == %p",(void*)current,(void*)m.eglContext);
  } else {
    VRB_LOG("*** EGLContext NOT EQUAL %p != %p",(void*)current,(void*)m.eglContext);
  }
  m.eglContext = current;

  m.resourcesHead.InitializeGL(*this);
  return true;
}


void
Context::Update() {
  if (m.addedResourcesHead.Update(*this)) {
    m.resourcesTail.PrependAndAdoptList(m.addedResourcesHead, m.addedResourcesTail);
  }
  m.updatableHead.UpdateResource(*this);
}

void
Context::ShutdownGL() {
  m.resourcesHead.ShutdownGL(*this);
  m.eglContext = EGL_NO_CONTEXT;
}

FileReaderPtr
Context::GetFileReader() {
#if defined(ANDROID)
  return m.fileReader;
#else
#  error "Platform not supported"
#endif // defined(ANDROID)
}

void
Context::AddUpdatable(Updatable* aUpdatable) {
  m.updatableTail.Prepend(aUpdatable);
}

void
Context::AddResourceGL(ResourceGL* aResource) {
  m.addedResourcesTail.Prepend(aResource);
}

TextureCachePtr
Context::GetTextureCache() {
  return m.textureCache;
}

#if defined(ANDROID)
SurfaceTextureFactoryPtr
Context::GetSurfaceTextureFactory() {
  return m.surfaceTextureFactory;
}
#endif // defined(ANDROID)

Context::Context(State& aState) : m(aState) {}
Context::~Context() {}

} // namespace vrb
