#ifndef VRB_RENDER_STATE_DOT_H
#define VRB_RENDER_STATE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/ResourceGL.h"

#include <GLES2/gl2.h>

namespace vrb{

class RenderState : protected ResourceGL {
public:
  static RenderStatePtr Create(ContextWeak& aContext);
  GLuint Program() const;
  GLint AttributePosition() const;
  GLint AttributeNormal() const;
  GLint AttributeUV() const;
  uint32_t GetLightId() const;
  void ResetLights(const uint32_t aId);
  void AddLight(const Vector& aDirection, const Color& aAmbient, const Color& aDiffuse, const Color& aSpecular);
  void SetMaterial(const Color& aAmbient, const Color& aDiffuse, const Color& aSpecular, const float aSpecularExponent);
  void SetTexture(const TexturePtr& aTexture);
  bool HasTexture() const;
  bool Enable(const Matrix& aPerspective, const Matrix& aView, const Matrix& aModel);
  void Disable();
protected:
  struct State;
  RenderState(State& aState, ContextWeak& aContext);
  ~RenderState();

  // ResourceGL interface
  void InitializeGL(Context& aContext) override;
  void ShutdownGL(Context& aContext) override;

  // RenderState interface
  GLuint LoadShader(GLenum type, const char* src);

private:
  State& m;
  RenderState() = delete;
  VRB_NO_DEFAULTS(RenderState)
};

} // namespace vrb

#endif //VRB_RENDER_STATE_DOT_H
