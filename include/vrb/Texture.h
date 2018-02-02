#ifndef VRB_TEXTURE_DOT_H
#define VRB_TEXTURE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include <GLES2/gl2.h>
#include <string>

namespace vrb {

class Texture {
public:
  static TexturePtr Create(ContextWeak& aContext);

  void Bind();
  void Unbind();
  std::string GetName() const;
  void SetName(const std::string& aName);
protected:
  struct State;
  Texture(State& aState, ContextWeak& aContext);
  virtual ~Texture();

  virtual void AboutToBind() {}

private:
  State& m;
  Texture() = delete;
  VRB_NO_DEFAULTS(Texture)
};

} // namespace vrb

#endif // VRB_TEXTURE_DOT_H
