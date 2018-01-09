#include "vrb/Texture.h"

#include "vrb/ConcreteClass.h"
#include "vrb/GLError.h"
#include "vrb/private/ResourceGLState.h"

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
  std::unique_ptr<uint8_t[]> data;

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
      , dataSize(0) {
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
    data = std::move(aSource.data);
    return *this;
  }

  void SetAlpha(const bool aHasAlpha) {
  	 internalFormat = aHasAlpha ? GL_RGBA : GL_RGB;
  	 format = aHasAlpha ? GL_RGBA : GL_RGB;
  }
private:
  MipMap(const MipMap&) = delete;
  MipMap& operator=(const MipMap&) = delete;
};

}

namespace vrb {

struct Texture::State : public ResourceGL::State {
  std::string name;
  GLuint texture;
  std::vector<MipMap> mipMaps;

  State() : texture(0) {}
};

TexturePtr
Texture::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<Texture, Texture::State> >(aContext);
}

void
Texture::SetName(const std::string& aName) {
  m.name = aName;
}

void
Texture::SetRGBData(std::unique_ptr<uint8_t[]>& aImage, const int aWidth, const int aHeight, const int aChannels) {
  if ((aChannels < 3) || (aChannels > 4)) {
    return;
  }

  if ((aWidth <= 0) || (aHeight <= 0)) {
    return;
  }

  if (!aImage) {
    return;
  }

  MipMap mipMap;
  mipMap.width = aWidth;
  mipMap.height = aHeight;
  mipMap.SetAlpha(aChannels == 4);
  mipMap.dataSize = aWidth * aHeight * aChannels;
  mipMap.data = std::move(aImage);
  m.mipMaps.push_back(std::move(mipMap));
}


std::string
Texture::GetName() {
  return m.name;
}

GLuint
Texture::GetHandle() {
  return m.texture;
}

Texture::Texture(State& aState, ContextWeak& aContext) : ResourceGL (aState, aContext), m(aState) {}
Texture::~Texture() {}

void
Texture::InitializeGL() {
  VRB_CHECK(glGenTextures(1, &(m.texture)));
  VRB_CHECK(glBindTexture(GL_TEXTURE_2D, m.texture));
  for (MipMap& mipMap: m.mipMaps) {
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
  	   (void*)mipMap.data.get()));
  }
}

void
Texture::ShutdownGL() {

}

} // namespace vrb
