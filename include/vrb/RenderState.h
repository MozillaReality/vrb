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
  bool Enable(const Matrix& aProjection);
protected:
  struct State;
  RenderState(State& aState, ContextWeak& aContext);
  ~RenderState();

  // ResourceGL interface
  void InitializeGL() override;
  void ShutdownGL() override;

  // RenderState interface
  GLuint LoadShader(GLenum type, const char* src);

private:
  State& m;
  RenderState() = delete;
  VRB_NO_DEFAULTS(RenderState)
};

} // namespace vrb

#endif //VRB_RENDER_STATE_DOT_H
