#ifndef VRB_TEXTURE_SURFACE_DOT_H
#define VRB_TEXTURE_SURFACE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/Texture.h"

#include <GLES2/gl2.h>
#include <string>

namespace vrb {

class TextureSurface : public Texture {
public:
  static TextureSurfacePtr Create(ContextWeak& aContext, const std::string& aName);

  void SetTextureHandle(const GLuint aHandle);

protected:
  struct State;
  TextureSurface(State& aState, ContextWeak& aContext);
  ~TextureSurface();

  // Texture interface
  void AboutToBind() override;

private:
  State& m;
  TextureSurface() = delete;
  VRB_NO_DEFAULTS(TextureSurface)
};

} // namespace vrb

#endif // VRB_TEXTURE_SURFACE_DOT_H
