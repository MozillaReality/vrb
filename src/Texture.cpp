#include "vrb/Texture.h"

#include "vrb/Base.h"
#include "vrb/GLError.h"

#include <GLES2/gl2.h>
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
  char* data;

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
  	   , data(nullptr)
  {}

  MipMap(const MipMap& aSource)
      : target(GL_TEXTURE_2D)
      , level(0)
  	   , internalFormat(GL_RGB)
  	   , width(0)
  	   , height(0)
  	   , border(0)
  	   , format(GL_RGB)
  	   , type(GL_UNSIGNED_BYTE)
      , dataSize(0)
      , data(nullptr) {
    *this = aSource;
  }

  ~MipMap() {
    delete[] data;
  }

  MipMap& operator=(const MipMap& aSource) {
    target = aSource.target;
    level = aSource.level;
    internalFormat = aSource.internalFormat;
    width = aSource.width;
    height = aSource.height;
    border = aSource.border;
    format = aSource.format;
    type = aSource.type;
    SetData(aSource.data, aSource.dataSize);
    return *this;
  }

  void SetData(const char* aData, const size_t aDataSize) {
    if (data) {
      delete data; data = nullptr;
      dataSize = 0;
    }

    if (aData && (aDataSize > 0)) {
      dataSize = aDataSize;
      data = new char[dataSize];
      memcpy((void*)data, (void*)aData, dataSize);
    }
  }

  void ResetRGB(const bool aHasAlpha) {
    target = GL_TEXTURE_2D;
    level = 0;
  	 internalFormat = aHasAlpha ? GL_RGBA : GL_RGB;
  	 width = 0;
  	 height = 0;
  	 border = 0;
  	 format = aHasAlpha ? GL_RGBA : GL_RGB;
  	 type = GL_UNSIGNED_BYTE;
    SetData(nullptr, 0);
  }
};

}

namespace vrb {

struct Texture::State {
  std::string name;
  GLuint texture;
  std::vector<MipMap> mipMaps;

  State() : texture(0) {}
};

TexturePtr
Texture::Create() {
  return std::make_shared<Alloc<Texture, Texture::State> >();
}

void
Texture::SetName(const std::string& aName) {
  m->name = aName;
}

void
Texture::SetRGBData(const void* aImage, const int aWidth, const int aHeight, const int aChannels) {
  if ((aChannels < 3) || (aChannels > 4)) {
    return;
  }

  if ((aWidth <= 0) || (aHeight <= 0)) {
    return;
  }

  if (!aImage) {
    return;
  }

  if (m->mipMaps.size() == 0) {
    m->mipMaps.push_back(MipMap());
  }

  MipMap& mipMap = m->mipMaps[0];
  mipMap.ResetRGB(aChannels == 4);
  mipMap.SetData((const char*)aImage, aWidth * aHeight * aChannels);
}

void
Texture::Init() {
  VRB_CHECK(glGenTextures(1, &(m->texture)));
  VRB_CHECK(glBindTexture(GL_TEXTURE_2D, m->texture));
  for (MipMap& mipMap: m->mipMaps) {
    if (!mipMap.data) {
      continue;
    }

    VRB_CHECK(glTexImage2D(
      mipMap.target,
  	   mipMap.level,
  	   mipMap.internalFormat,
  	   mipMap.width,
  	   mipMap.height,
  	   mipMap.border,
  	   mipMap.format,
  	   mipMap.type,
  	   (void*)mipMap.data));
  }
}

std::string
Texture::GetName() {
  return m->name;
}

GLuint
Texture::GetHandle() {
  return m->texture;
}

Texture::Texture() : m(nullptr) {}

Texture::~Texture() {}

} // namespace vrb
