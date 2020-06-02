/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/RenderContext.h"
#include "vrb/ConcreteClass.h"
#include "vrb/private/ResourceGLState.h"
#include "vrb/private/UpdatableState.h"

#include "vrb/ContextSynchronizer.h"
#include "vrb/CreationContext.h"
#if defined(ANDROID)
#  include "vrb/ClassLoaderAndroid.h"
#  include "vrb/FileReaderAndroid.h"
#else
#  include "vrb/FileReaderBasic.h"
#endif // defined(ANDROID)
#include "vrb/DataCache.h"
#include "vrb/GLExtensions.h"
#include "vrb/Logger.h"
#include "vrb/ProgramFactory.h"
#include "vrb/ResourceGL.h"
#if defined(ANDROID)
#  include "vrb/SurfaceTextureFactory.h"
#endif // defined(ANDROID)
#include "vrb/TextureCache.h"
#include "vrb/ThreadIdentity.h"
#include "vrb/Updatable.h"
#if defined(ANDROID)
#  include <EGL/egl.h>
#endif // defined(ANDROID)
#include <pthread.h>
#include <time.h>
#include <vector>

namespace {
const double kNanosecondsToSeconds = 1.0e9;
}

namespace vrb {

struct RenderContext::State {
  ThreadIdentityPtr threadSelf;
  TextureCachePtr textureCache;
  ProgramFactoryPtr programFactory;
  DataCachePtr dataCache;
  CreationContextPtr creationContext;
  GLExtensionsPtr glExtensions;
#if defined(ANDROID)
  EGLContext eglContext;
  FileReaderAndroidPtr fileReader;
  SurfaceTextureFactoryPtr surfaceTextureFactory;
  ClassLoaderAndroidPtr classLoader;
#else
  FileReaderBasicPtr fileReader;
#endif // defined(ANDROID)
  UpdatableList updatables;
  ResourceGLList uninitializedResources;
  ResourceGLList resources;
  std::vector<ContextSynchronizerPtr> synchronizers;
  double timestamp;
  double frameDelta;
  State();
};

RenderContext::State::State()
    : threadSelf(ThreadIdentity::Create())
#if defined(ANDROID)
    , eglContext(EGL_NO_CONTEXT)
#endif // defined(ANDROID)
    , dataCache(DataCache::Create())
    , textureCache(TextureCache::Create())
    , programFactory(ProgramFactory::Create())
    , timestamp(0.0)
    , frameDelta(0.0)
{}

RenderContextPtr
RenderContext::Create() {
  RenderContextPtr result = std::make_shared<ConcreteClass<RenderContext, RenderContext::State> >();
  result->m.creationContext = CreationContext::Create(result);
  result->m.creationContext->BindToThread();
  result->m.textureCache->Init(result->m.creationContext);
  result->m.glExtensions = GLExtensions::Create(result);
#if defined(ANDROID)
  result->m.surfaceTextureFactory = SurfaceTextureFactory::Create(result->m.creationContext);
  result->m.fileReader = FileReaderAndroid::Create();
  result->m.classLoader = ClassLoaderAndroid::Create();
#else
  result->m.fileReader = FileReaderBasic::Create();
#endif // defined(ANDROID)
  result->m.creationContext->SetFileReader(result->m.fileReader);
  return result;
}

#if defined(ANDROID)
void
RenderContext::InitializeJava(JNIEnv* aEnv, jobject& aActivity, jobject& aAssetManager) {
  if (m.classLoader) { m.classLoader->Init(aEnv, aActivity); }
  if (m.fileReader) { m.fileReader->Init(aEnv, aAssetManager, m.classLoader); }
  if (m.surfaceTextureFactory) { m.surfaceTextureFactory->InitializeJava(aEnv); }
}

void
RenderContext::ShutdownJava() {
  if (m.fileReader) { m.fileReader->Shutdown(); }
  if (m.classLoader) { m.classLoader->Shutdown(); }
}
#endif // defined(ANDROID)

bool
RenderContext::IsOnRenderThread() {
  return m.threadSelf->IsOnInitializationThread();
}

bool
RenderContext::InitializeGL() {
#if defined(ANDROID)
  EGLContext current = eglGetCurrentContext();
  if (current == EGL_NO_CONTEXT) {
    VRB_WARN("Unable to initialize VRB context: EGLContext is not valid.");
    m.eglContext = current;
    return false;
  }
  if (current == m.eglContext) {
    VRB_ERROR("RenderContext already initialized.");
  }
  m.eglContext = current;
#endif // defined(ANDROID)
  m.resources.InitializeGL();
  m.glExtensions->Initialize();
  return true;
}

void
RenderContext::ShutdownGL() {
  m.resources.ShutdownGL();
}

void
RenderContext::Update() {
  timespec spec = {};
  if (clock_gettime(CLOCK_MONOTONIC, &spec) == 0) {
    double nextTimestamp = (double)spec.tv_sec + (spec.tv_nsec / kNanosecondsToSeconds);
    if (m.timestamp != 0.0) {
      m.frameDelta = nextTimestamp - m.timestamp;
    }
    m.timestamp = nextTimestamp;
  }
  m.creationContext->Synchronize();
  for(auto iter = m.synchronizers.begin(); iter != m.synchronizers.end();) {
    bool active = true;
    (*iter)->Signal(active);
    if(!active) {
      iter = m.synchronizers.erase(iter);
    } else {
      iter++;
    }
  }
  if (m.uninitializedResources.Update()) {
    m.resources.AppendAndAdoptList(m.uninitializedResources);
  }
  m.updatables.UpdateResource(*this);
}

double
RenderContext::GetTimestamp() {
  return m.timestamp;
}

double
RenderContext::GetFrameDelta() {
  return m.frameDelta;
}

ThreadIdentityPtr&
RenderContext::GetRenderThreadIdentity() {
  return m.threadSelf;
}

DataCachePtr&
RenderContext::GetDataCache() {
  return m.dataCache;
}

TextureCachePtr&
RenderContext::GetTextureCache() {
  return m.textureCache;
}

ProgramFactoryPtr&
RenderContext::GetProgramFactory() {
  return m.programFactory;
}


CreationContextPtr&
RenderContext::GetRenderThreadCreationContext() {
  return m.creationContext;
}

GLExtensionsPtr
RenderContext::GetGLExtensions() const {
  return m.glExtensions;
}

#if defined(ANDROID)
SurfaceTextureFactoryPtr
RenderContext::GetSurfaceTextureFactory() {
  return m.surfaceTextureFactory;
}

#endif // defined(ANDROID)

// Internal interface
ResourceGLList&
RenderContext::GetUninitializedResourceGLList() {
  return m.uninitializedResources;
}

ResourceGLList&
RenderContext::GetResourceGLList() {
  return m.resources;
}

UpdatableList&
RenderContext::GetUpdatableList() {
  return m.updatables;
}

void
RenderContext::RegisterContextSynchronizer(ContextSynchronizerPtr& aSynchronizer) {
  m.synchronizers.push_back(aSynchronizer);
}

RenderContext::RenderContext(State& aState) : m(aState) {}
RenderContext::~RenderContext() {}

} // namespace vrb
