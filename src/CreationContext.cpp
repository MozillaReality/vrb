/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/CreationContext.h"
#include "vrb/ConcreteClass.h"

#include "vrb/ContextSynchronizer.h"
#include "vrb/DataCache.h"
#include "vrb/FileReader.h"
#include "vrb/Logger.h"
#include "vrb/RenderContext.h"
#include "vrb/TextureCache.h"
#include "vrb/TextureGL.h"

#include "vrb/private/ResourceGLState.h"
#include "vrb/private/UpdatableState.h"

#include <pthread.h>

#define ASSERT_ON_CREATION_THREAD()                                          \
  if (pthread_equal(m.threadSelf, pthread_self()) == 0) {                    \
    VRB_ERROR("CreationContext::%s called on wrong thread", __FUNCTION__);     \
    return;                                                                  \
  }

namespace {

class TextureHandler;
typedef std::shared_ptr<TextureHandler> TextureHandlerPtr;

class TextureHandler : public vrb::FileHandler {
public:
  static TextureHandlerPtr Create(const vrb::TextureGLPtr& aTexture);
  void BindFileHandle(const std::string& aFileName, const int aFileHandle) override;
  void LoadFailed(const int aFileHandle, const std::string& aReason) override;
  void ProcessRawFileChunk(const int aFileHandle, const char* aBuffer, const size_t aSize) override {};
  void FinishRawFile(const int aFileHandle) override {};
  void ProcessImageFile(const int aFileHandle, std::unique_ptr<uint8_t[]>& aImage, const int aWidth, const int aHeight) override;
  TextureHandler() {}
  ~TextureHandler() {}
protected:
  vrb::TextureGLPtr mTexture;
private:
  VRB_NO_DEFAULTS(TextureHandler)
};

TextureHandlerPtr
TextureHandler::Create(const vrb::TextureGLPtr& aTexture) {
  TextureHandlerPtr result = std::make_shared<TextureHandler>();
  result->mTexture = aTexture;
  return result;
}

void
TextureHandler::BindFileHandle(const std::string& aFileName, const int aFileHandle) {

}

void
TextureHandler::LoadFailed(const int aFileHandle, const std::string& aReason) {
  VRB_ERROR("Failed to load texture: %s", aReason.c_str());
}

void
TextureHandler::ProcessImageFile(const int aFileHandle, std::unique_ptr<uint8_t[]>& aImage, const int aWidth, const int aHeight) {
  if (mTexture) {
    mTexture->SetRGBData(aImage, aWidth, aHeight, 4);
  }
}

}

namespace vrb {

struct CreationContext::State {
  CreationContextWeak self;
  ContextSynchronizerPtr sync;
  ResourceGLList uninitializedResources;
  ResourceGLList resources;
  UpdatableList updatables;
  FileReaderPtr fileReader;
  DataCachePtr dataCache;
  TextureCachePtr textureCache;
  pthread_t threadSelf;

  State() {}
};

CreationContextPtr
CreationContext::Create(RenderContextPtr& aContext) {
  if (!aContext->IsOnRenderThread()) {
    VRB_ERROR("Creation Context must be created on render thread");
    return nullptr;
  }
  CreationContextPtr result = std::make_shared<ConcreteClass<CreationContext, CreationContext::State> >();
  result->m.self = result;
  result->m.sync = ContextSynchronizer::Create(aContext);
  result->m.dataCache = aContext->GetDataCache();
  result->m.textureCache = aContext->GetTextureCache();
  return result;
}

void
CreationContext::RegisterContextSynchronizerObserver(ContextSynchronizerObserverPtr& aObserver) {
  m.sync->RegisterObserver(aObserver);
}

void
CreationContext::ReleaseContextSynchronizerObserver(ContextSynchronizerObserverPtr& aObserver) {
  m.sync->ReleaseObserver(aObserver);
}

void
CreationContext::BindToThread() {
  VRB_LOG("CreationContext::BindToThread()");
  m.threadSelf = pthread_self();
  m.sync->BindToThread();
}

void
CreationContext::Synchronize() {
  ASSERT_ON_CREATION_THREAD();
  if (m.uninitializedResources.IsDirty() || m.resources.IsDirty() || m.updatables.IsDirty()) {
    m.sync->AdoptLists(m.uninitializedResources, m.resources, m.updatables);
  }
}

void
CreationContext::SetFileReader(FileReaderPtr aFileReader) {
  m.fileReader = std::move(aFileReader);
}

DataCachePtr
CreationContext::GetDataCache() {
  return m.dataCache;
}

FileReaderPtr
CreationContext::GetFileReader() {
  return m.fileReader;
}

TextureGLPtr
CreationContext::LoadTexture(const std::string& aTextureName, const bool aUseCache) {
  TextureGLPtr result;
  if (aUseCache) {
    result = m.textureCache->FindTexture(aTextureName);
    if (result) {
      return result;
    }
  }

  if (!m.fileReader) {
    return m.textureCache->GetDefaultTexture();
  }

  CreationContextPtr context = m.self.lock();
  if (!context) {
    return m.textureCache->GetDefaultTexture();
  }
  result = TextureGL::Create(context);
  m.textureCache->AddTexture(aTextureName, result);
  m.fileReader->ReadImageFile(aTextureName, TextureHandler::Create(result));
  result->SetName(aTextureName);

  return result;
}

void
CreationContext::UpdateResourceGL() {
  ResourceGLList list;
  m.uninitializedResources.GetOffRenderThreadResources(list);
  if (list.Update()) {
    m.resources.AppendAndAdoptList(list);
  }
}

void
CreationContext::AddResourceGL(ResourceGL* aResource) {
  ASSERT_ON_CREATION_THREAD();
  m.uninitializedResources.Append(aResource);
}

void
CreationContext::AddUpdatable(Updatable* aUpdatable) {
  ASSERT_ON_CREATION_THREAD();
  m.updatables.Append(aUpdatable);
}

CreationContext::CreationContext(State& aState) : m(aState) {}
CreationContext::~CreationContext() {
  m.sync->Release();
}

} // namespace vrb
