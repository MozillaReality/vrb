#ifndef VRB_TEXTURE_DOT_H
#define VRB_TEXTURE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/ResourceGL.h"

#include <GLES2/gl2.h>
#include <string>

namespace vrb {

class Texture : protected ResourceGL {
public:
  static TexturePtr Create(ContextWeak& aContext);

  void SetName(const std::string& aName);
  void SetRGBData(std::unique_ptr<uint8_t[]>& aImage, const int aWidth, const int aHeight, const int aChannels);
  std::string GetName();
  GLuint GetHandle();
protected:
  struct State;
  Texture(State& aState, ContextWeak& aContext);
  ~Texture();

  // From ResourceGL
  void InitializeGL() override;
  void ShutdownGL() override;

private:
  State& m;
  Texture() = delete;
  VRB_NO_DEFAULTS(Texture)
};

} // namespace vrb

#endif // VRB_TEXTURE_DOT_H
