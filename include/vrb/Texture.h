#ifndef VRB_TEXTURE_DOT_H
#define VRB_TEXTURE_DOT_H

#include "vrb/Forward.h"

#include <GLES2/gl2.h>
#include <string>

namespace vrb {

class Texture {
public:
  static TexturePtr Create();

  void SetName(const std::string& aName);
  void SetRGBData(const void* aImage, const int aWidth, const int aHeight, const int aChannels);
  void Init();
  std::string GetName();
  GLuint GetHandle();

protected:
  Texture();
  ~Texture();

  struct State;
  State* m;

private:
  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;
};

} // namespace vrb

#endif // VRB_TEXTURE_DOT_H
