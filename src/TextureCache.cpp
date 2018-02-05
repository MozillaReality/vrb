/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/TextureCache.h"

#include "vrb/ConcreteClass.h"
#include "vrb/Context.h"
#include "vrb/DefaultImageData.h"
#include "vrb/FileReader.h"
#include "vrb/Logger.h"
#include "vrb/Texture.h"
#include "vrb/TextureGL.h"

#include <unordered_map>

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

struct TextureCache::State {
  ContextWeak context;
  TextureGLPtr defaultTexture;
  std::unordered_map<std::string, TextureGLPtr> cache;

};
TextureCachePtr
TextureCache::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<TextureCache, TextureCache::State> >(aContext);
}

void
TextureCache::Init() {

}

void
TextureCache::Shutdown() {

}

TexturePtr
TextureCache::LoadTexture(const std::string& aFileName) {
  TextureGLPtr result;

  std::unordered_map<std::string, TextureGLPtr>::iterator it = m.cache.find(aFileName);
  if (it != m.cache.end()) {
    return it->second;
  }

  ContextPtr context = m.context.lock();
  if (!context) {
    return m.defaultTexture;
  }

  FileReaderPtr reader = context->GetFileReader();

  if (!reader) {
    return m.defaultTexture;
  }

  result = TextureGL::Create(m.context);
  m.cache[aFileName] = result;
  reader->ReadImageFile(aFileName, TextureHandler::Create(result));

  return result;
}

TexturePtr
TextureCache::GetDefaultTexture() {
  return m.defaultTexture;
}

TextureCache::TextureCache(State& aState, ContextWeak& aContext) : m(aState) {
  m.context = aContext;
  m.defaultTexture = TextureGL::Create(aContext);
  const size_t kArraySize = kDefaultImageDataSize * sizeof(uint32_t);
  std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(kArraySize);
  memcpy(data.get(), (void*)kDefaultImageData, kArraySize);
  m.defaultTexture->SetRGBData(data, kDefaultImageDataWidth, kDefaultImageDataHeight, 4);
}

TextureCache::~TextureCache() {}

} // namespace vrb
