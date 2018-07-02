#include "vrb/CreationContext.h"
#include "vrb/ConcreteClass.h"

#include "vrb/ContextSynchronizer.h"
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
    VRB_LOG("CreationContext::%s called on wrong thread", __FUNCTION__);     \
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
  VRB_LOG("Failed to load texture: %s", aReason.c_str());
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
  ResourceGLHead resourceHead;
  ResourceGLTail resourceTail;
  UpdatableHead updatableHead;
  UpdatableTail updatableTail;
  FileReaderPtr fileReader;
  TextureCachePtr textureCache;
  pthread_t threadSelf;

  State() {
    resourceHead.BindTail(resourceTail);
    updatableHead.BindTail(updatableTail);
  }
};

CreationContextPtr
CreationContext::Create(RenderContextPtr& aContext) {
  if (!aContext->IsOnRenderThread()) {
    VRB_LOG("Creation Context must be created on render thread");
    return nullptr;
  }
  CreationContextPtr result = std::make_shared<ConcreteClass<CreationContext, CreationContext::State> >();
  result->m.self = result;
  result->m.sync = ContextSynchronizer::Create(aContext);
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
  if (m.resourceHead.IsDirty(m.resourceTail) || m.updatableHead.IsDirty(m.updatableTail)) {
    m.sync->AdoptLists(m.resourceHead, m.resourceTail, m.updatableHead, m.updatableTail);
  }
}

void
CreationContext::SetFileReader(FileReaderPtr aFileReader) {
  m.fileReader = std::move(aFileReader);
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

  return result;
}


void
CreationContext::AddResourceGL(ResourceGL* aResource) {
  ASSERT_ON_CREATION_THREAD();
  m.resourceTail.Prepend(aResource);
}

void
CreationContext::AddUpdatable(Updatable* aUpdatable) {
  ASSERT_ON_CREATION_THREAD();
  m.updatableTail.Prepend(aUpdatable);
}

CreationContext::CreationContext(State& aState) : m(aState) {}
CreationContext::~CreationContext() {
  m.sync->Release();
}

} // namespace vrb
