#ifndef VRB_FBO_DOT_H
#define VRB_FBO_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include "vrb/gl.h"

namespace vrb {

class FBO {
public:
  struct Attributes {
    Attributes();
    Attributes(bool aDepth, bool aMultiview, int aSamples);
    bool depth;
    bool multiview;
    int samples;
  };
  static FBOPtr Create(ContextWeak& aContext);
  bool IsValid() const;
  void SetTextureHandle(const GLuint aHandle,
                        const int32_t aWidth,
                        const int32_t aHeight,
                        const FBO::Attributes& aAttributtes = {});
  void Bind();
  void Unbind();
  const FBO::Attributes& GetAttributes() const;
protected:
  struct State;
  FBO(State& aState, ContextWeak& aContext);
  ~FBO();
private:
  State& m;
  FBO() = delete;
  VRB_NO_DEFAULTS(FBO)
};

}
#endif // VRB_FBO_DOT_H
