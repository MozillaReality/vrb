#ifndef VRB_TEXTURE_GL_DOT_H
#define VRB_TEXTURE_GL_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/ResourceGL.h"
#include "vrb/Texture.h"

#include <GLES2/gl2.h>
#include <string>

namespace vrb {

class TextureGL : public Texture, protected ResourceGL {
public:
  static TextureGLPtr Create(ContextWeak& aContext);

  void SetRGBData(std::unique_ptr<uint8_t[]>& aImage, const int aWidth, const int aHeight, const int aChannels);
protected:
  struct State;
  TextureGL(State& aState, ContextWeak& aContext);
  ~TextureGL();

  // Texture interface
  void AboutToBind() override;

  // ResourceGL interface
  void InitializeGL(Context& aContext) override;
  void ShutdownGL(Context& aContext) override;

private:
  State& m;
  TextureGL() = delete;
  VRB_NO_DEFAULTS(TextureGL)
};

} // namespace vrb

#endif // VRB_TEXTURE_GL_DOT_H
