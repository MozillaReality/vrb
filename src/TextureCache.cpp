/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/TextureCache.h"
#include "vrb/ConcreteClass.h"

#include "vrb/CreationContext.h"
#include "vrb/DefaultImageData.h"
#include "vrb/FileReader.h"
#include "vrb/Logger.h"
#include "vrb/Mutex.h"
#include "vrb/Texture.h"
#include "vrb/TextureGL.h"

#include <unordered_map>

namespace vrb {

struct TextureCache::State {
  Mutex lock;
  TextureGLPtr defaultTexture;
  std::unordered_map<std::string, TextureGLPtr> cache;

};
TextureCachePtr
TextureCache::Create() {
  return std::make_shared<ConcreteClass<TextureCache, TextureCache::State> >();
}

void
TextureCache::Init(CreationContextPtr& aContext) {
  MutexAutoLock lock(m.lock);
  m.defaultTexture = TextureGL::Create(aContext);
  const size_t kArraySize = kDefaultImageDataSize * sizeof(uint32_t);
  std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(kArraySize);
  memcpy(data.get(), (void*)kDefaultImageData, kArraySize);
  m.defaultTexture->SetRGBData(data, kDefaultImageDataWidth, kDefaultImageDataHeight, 4);
}

void
TextureCache::Shutdown() {
  m.defaultTexture = nullptr;
  m.cache.clear();
}

TextureGLPtr
TextureCache::FindTexture(const std::string& aTextureName) {
  MutexAutoLock lock(m.lock);
  TextureGLPtr result;

  std::unordered_map<std::string, TextureGLPtr>::iterator it = m.cache.find(aTextureName);
  if (it != m.cache.end()) {
    return it->second;
  }

  return result;
}

void
TextureCache::AddTexture(const std::string& aTextureName, TextureGLPtr& aTexture) {
  MutexAutoLock lock(m.lock);
  m.cache[aTextureName] = aTexture;
}

TextureGLPtr
TextureCache::GetDefaultTexture() {
  return m.defaultTexture;
}

TextureCache::TextureCache(State& aState) : m(aState) {}

TextureCache::~TextureCache() {}

} // namespace vrb
