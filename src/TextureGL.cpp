/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/TextureGL.h"
#include "vrb/private/TextureState.h"

#include "vrb/ConcreteClass.h"
#include "vrb/CreationContext.h"
#include "vrb/DataCache.h"
#include "vrb/GLError.h"
#include "vrb/Logger.h"
#include "vrb/private/ResourceGLState.h"

#include "vrb/gl.h"
#include <cstring>
#include <vector>

namespace {

struct MipMap {
  GLenum target;
  GLint level;
  GLint internalFormat;
  GLsizei width;
  GLsizei height;
  GLint border;
  GLenum format;
  GLenum type;
  GLsizei dataSize;
  std::unique_ptr<uint8_t[]> data;
  uint32_t dataCacheHandle;

  MipMap()
      : target(GL_TEXTURE_2D)
      , level(0)
      , internalFormat(GL_RGB)
      , width(0)
      , height(0)
      , border(0)
      , format(GL_RGB)
      , type(GL_UNSIGNED_BYTE)
      , dataSize(0)
      , dataCacheHandle(0)
  {}

  MipMap(MipMap&& aSource)
      : target(GL_TEXTURE_2D)
      , level(0)
      , internalFormat(GL_RGB)
      , width(0)
      , height(0)
      , border(0)
      , format(GL_RGB)
      , type(GL_UNSIGNED_BYTE)
      , dataSize(0)
      , dataCacheHandle(0)
  {
    *this = std::move(aSource);
  }

  ~MipMap() {
  }

  MipMap& operator=(MipMap&& aSource) {
    target = aSource.target;
    level = aSource.level;
    internalFormat = aSource.internalFormat;
    width = aSource.width;
    height = aSource.height;
    border = aSource.border;
    format = aSource.format;
    type = aSource.type;
    dataSize = aSource.dataSize;
    dataCacheHandle = aSource.dataCacheHandle;
    data = std::move(aSource.data);
    return *this;
  }

private:
  MipMap(const MipMap&) = delete;
  MipMap& operator=(const MipMap&) = delete;
};

}

namespace vrb {

struct TextureGL::State : public Texture::State, public ResourceGL::State {
  bool dirty;
  DataCachePtr dataCache;
  std::vector<MipMap> mipMaps;

  State() : dirty(false) {}
  void CreateTexture();
  void DestroyTexture();
};

void
TextureGL::State::CreateTexture() {
  if (!dirty) {
    return;
  }
  VRB_GL_CHECK(glGenTextures(1, &texture));
  VRB_GL_CHECK(glBindTexture(target, texture));
  for (MipMap& mipMap: mipMaps) {
    if (dataCache && (mipMap.dataCacheHandle > 0)) {
      dataCache->LoadData(mipMap.dataCacheHandle, mipMap.data);
    }
    if (!mipMap.data) {
      continue;
    }

    if (mipMap.format == GL_RG8 || mipMap.format == GL_RGBA) {
      VRB_GL_CHECK(glTexImage2D(
          mipMap.target,
          mipMap.level,
          mipMap.internalFormat,
          mipMap.width,
          mipMap.height,
          mipMap.border,
          mipMap.format,
          mipMap.type,
          (void*)mipMap.data.get()));
    } else {
      VRB_GL_CHECK(glCompressedTexImage2D(
          mipMap.target,
          mipMap.level,
          mipMap.internalFormat,
          mipMap.width,
          mipMap.height,
          mipMap.border,
          mipMap.dataSize,
          (void*)mipMap.data.get()));
    }

    if (dataCache) {
      if (mipMap.dataCacheHandle == 0) {
        mipMap.dataCacheHandle = dataCache->CacheData(mipMap.data, (size_t)mipMap.dataSize);
      } else {
        mipMap.data = nullptr;
      }
    }
  }

  for (auto param = intMap.begin(); param != intMap.end(); param++) {
    VRB_GL_CHECK(glTexParameteri(target, param->first, param->second));
  }
  dirty = false;
}

void
TextureGL::State::DestroyTexture() {
  if (texture > 0) {
    VRB_GL_CHECK(glDeleteTextures(1, &texture));
    texture = 0;
  }
  dirty = true;
}

TextureGLPtr
TextureGL::Create(CreationContextPtr& aContext) {
  return std::make_shared<ConcreteClass<TextureGL, TextureGL::State> >(aContext);
}

void
TextureGL::SetImageData(std::unique_ptr<uint8_t[]>& aImage, const uint64_t aImageLength, const int aWidth, const int aHeight, const GLenum aFormat) {
  if ((aWidth <= 0) || (aHeight <= 0)) {
    return;
  }

  if (!aImage) {
    return;
  }

  MipMap mipMap;
  mipMap.width = aWidth;
  mipMap.height = aHeight;
  mipMap.dataSize = (GLsizei) aImageLength;
  mipMap.data = std::move(aImage);
  mipMap.internalFormat = aFormat;
  mipMap.format = aFormat;
  m.mipMaps.clear();
  m.mipMaps.push_back(std::move(mipMap));
  m.dirty = true;
}

TextureGL::TextureGL(State& aState, CreationContextPtr& aContext) : Texture(aState, aContext), ResourceGL (aState, aContext), m(aState) {
  m.dataCache = aContext->GetDataCache();
}
TextureGL::~TextureGL() {
  if (!m.dataCache) {
    return;
  }
  for (MipMap& mipMap: m.mipMaps) {
    if (mipMap.dataCacheHandle > 0) {
      m.dataCache->RemoveData(mipMap.dataCacheHandle);
      mipMap.dataCacheHandle = 0;
    }
  }
}

GLsizei
TextureGL::GetWidth() const {
  if (m.mipMaps.size()) {
    return m.mipMaps.front().width;
  }
  return 0;
}

GLsizei
TextureGL::GetHeight() const {
  if (m.mipMaps.size()) {
    return m.mipMaps.front().height;
  }
  return 0;
}

void
TextureGL::AboutToBind() {
  m.CreateTexture();
}

bool
TextureGL::SupportOffRenderThreadInitialization() {
  return true;
}

void
TextureGL::InitializeGL() {
  m.CreateTexture();
}

void
TextureGL::ShutdownGL() {
  m.DestroyTexture();
}

} // namespace vrb
