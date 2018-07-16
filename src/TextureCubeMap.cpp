/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/TextureCubeMap.h"
#include "vrb/private/TextureState.h"
#include "vrb/ConcreteClass.h"

#include "vrb/CreationContext.h"
#include "vrb/DataCache.h"
#include "vrb/FileReader.h"
#include "vrb/GLError.h"
#include "vrb/Logger.h"
#include "vrb/private/ResourceGLState.h"
#include "vrb/RenderContext.h"

#include "vrb/gl.h"
#include <cstring>
#include <vector>

namespace {

struct CubeMapFace {
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


  CubeMapFace()
      : target(GL_TEXTURE_CUBE_MAP_POSITIVE_X)
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

  void SetAlpha(const bool aHasAlpha) {
    internalFormat = aHasAlpha ? GL_RGBA : GL_RGB;
    format = aHasAlpha ? GL_RGBA : GL_RGB;
  }
private:
  CubeMapFace(const CubeMapFace&) = delete;
  CubeMapFace& operator=(const CubeMapFace&) = delete;
};



class CubeMapTextureHandler;
typedef std::shared_ptr<CubeMapTextureHandler> CubeMapTextureHandlerPtr;

class CubeMapTextureHandler : public vrb::FileHandler {
public:
  static CubeMapTextureHandlerPtr Create(const vrb::TextureCubeMapPtr& aTexture, GLenum aFaceTarget);
  void BindFileHandle(const std::string& aFileName, const int aFileHandle) override;
  void LoadFailed(const int aFileHandle, const std::string& aReason) override;
  void ProcessRawFileChunk(const int aFileHandle, const char* aBuffer, const size_t aSize) override {};
  void FinishRawFile(const int aFileHandle) override {};
  void ProcessImageFile(const int aFileHandle, std::unique_ptr<uint8_t[]>& aImage, const int aWidth, const int aHeight) override;
  CubeMapTextureHandler() {}
  ~CubeMapTextureHandler() {}
protected:
  vrb::TextureCubeMapPtr mTexture;
  GLenum mFaceTarget;
private:
  VRB_NO_DEFAULTS(CubeMapTextureHandler);
};

CubeMapTextureHandlerPtr
CubeMapTextureHandler::Create(const vrb::TextureCubeMapPtr& aTexture, GLenum aFaceTarget) {
  CubeMapTextureHandlerPtr result = std::make_shared<CubeMapTextureHandler>();
  result->mTexture = aTexture;
  result->mFaceTarget = aFaceTarget;
  return result;
}

void
CubeMapTextureHandler::BindFileHandle(const std::string& aFileName, const int aFileHandle) {

}

void
CubeMapTextureHandler::LoadFailed(const int aFileHandle, const std::string& aReason) {
  VRB_ERROR("Failed to load CubeMap texture for target %d: %s", mFaceTarget, aReason.c_str());
}

void
CubeMapTextureHandler::ProcessImageFile(const int aFileHandle, std::unique_ptr<uint8_t[]>& aImage, const int aWidth, const int aHeight) {
  if (mTexture) {
    mTexture->SetRGBData(mFaceTarget, aImage, aWidth, aHeight, 4);
  }
}

}

namespace vrb {

struct TextureCubeMap::State : public Texture::State, public ResourceGL::State {
  bool dirty;
  CubeMapFace faces[6];
  DataCachePtr dataCache;

  void CreateTexture();
  void DestroyTexture();
};

void
TextureCubeMap::State::CreateTexture() {
  if (!dirty) {
    return;
  }
  for (CubeMapFace& face: faces) {
    if (!face.data) {
      if (dataCache && (face.dataCacheHandle > 0)) {
        dataCache->LoadData(face.dataCacheHandle, face.data);
      }
      if (!face.data) {
        return;
      }
    }
  }
  VRB_GL_CHECK(glGenTextures(1, &texture));
  VRB_GL_CHECK(glBindTexture(target, texture));
  for (CubeMapFace& face: faces) {
    VRB_GL_CHECK(glTexImage2D(
        face.target,
        face.level,
        face.internalFormat,
        face.width,
        face.height,
        face.border,
        face.format,
        face.type,
        (void*)face.data.get()));
    if (!face.dataCacheHandle && dataCache) {
      face.dataCacheHandle = dataCache->CacheData(face.data, (size_t)face.dataSize);
    } else if (face.dataCacheHandle > 0) {
      face.data = nullptr;
    }
  }

  for (auto param = intMap.begin(); param != intMap.end(); param++) {
    VRB_GL_CHECK(glTexParameteri(target, param->first, param->second));
  }
  dirty = false;
}

void
TextureCubeMap::State::DestroyTexture() {
  if (texture > 0) {
    VRB_GL_CHECK(glDeleteTextures(1, &texture));
    texture = 0;
  }
  dirty = true;
}

TextureCubeMapPtr
TextureCubeMap::Create(CreationContextPtr& aContext) {
  return std::make_shared<ConcreteClass<TextureCubeMap, TextureCubeMap::State> >(aContext);
}


void
TextureCubeMap::Load(CreationContextPtr& aContext, const TextureCubeMapPtr& aTexture,
                     const std::string& aFileXPos, const std::string& aFileXNeg,
                     const std::string& aFileYPos, const std::string& aFileYNeg,
                     const std::string& aFileZPos, const std::string& aFileZNeg) {
  FileReaderPtr reader = aContext->GetFileReader();

  if (!reader) {
    VRB_ERROR("FileReaderPtr not found while loading a CubeMap");
    return;
  }

  reader->ReadImageFile(aFileXPos, CubeMapTextureHandler::Create(aTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_X));
  reader->ReadImageFile(aFileXNeg, CubeMapTextureHandler::Create(aTexture, GL_TEXTURE_CUBE_MAP_NEGATIVE_X));
  reader->ReadImageFile(aFileYPos, CubeMapTextureHandler::Create(aTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_Y));
  reader->ReadImageFile(aFileYNeg, CubeMapTextureHandler::Create(aTexture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y));
  reader->ReadImageFile(aFileZPos, CubeMapTextureHandler::Create(aTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_Z));
  reader->ReadImageFile(aFileZNeg, CubeMapTextureHandler::Create(aTexture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z));
}

void
TextureCubeMap::SetRGBData(const GLenum aFaceTarget, std::unique_ptr<uint8_t[]>& aImage, const int aWidth, const int aHeight, const int aChannels) {
  if ((aChannels < 3) || (aChannels > 4)) {
    return;
  }

  if ((aWidth <= 0) || (aHeight <= 0)) {
    return;
  }

  if (!aImage) {
    return;
  }

  int index = aFaceTarget - GL_TEXTURE_CUBE_MAP_POSITIVE_X;
  if (index < 0 || index >= 6) {
    return;
  }

  CubeMapFace& face = m.faces[index];
  face.width = aWidth;
  face.height = aHeight;
  face.SetAlpha(aChannels == 4);
  face.dataSize = aWidth * aHeight * aChannels;
  face.data = std::move(aImage);
  m.dirty = true;
}

TextureCubeMap::TextureCubeMap(State& aState, CreationContextPtr& aContext) : Texture(aState, aContext), ResourceGL (aState, aContext), m(aState) {
  m.dataCache = aContext->GetDataCache();
  m.target = GL_TEXTURE_CUBE_MAP;
  for (int i = 0; i < 6; ++i) {
    m.faces[i].target = (GLenum) GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
  }
}

TextureCubeMap::~TextureCubeMap() {
  if (!m.dataCache) {
    return;
  }
  for (CubeMapFace& face: m.faces) {
    if (face.dataCacheHandle > 0) {
      m.dataCache->RemoveData(face.dataCacheHandle);
      face.dataCacheHandle = 0;
    }
  }
}

void
TextureCubeMap::AboutToBind() {
  m.CreateTexture();
}

bool
TextureCubeMap::SupportOffRenderThreadInitialization() {
  return true;
}

void
TextureCubeMap::InitializeGL() {
  m.CreateTexture();
}

void
TextureCubeMap::ShutdownGL() {
  m.DestroyTexture();
}

} // namespace vrb
