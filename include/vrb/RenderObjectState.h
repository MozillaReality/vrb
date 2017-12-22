#ifndef VRB_RENDER_OBJECT_STATE_DOT_H
#define VRB_RENDER_OBJECT_STATE_DOT_H

#include "vrb/Forward.h"

#include <GLES2/gl2.h>

namespace vrb{

class RenderObjectState {
public:
  static RenderObjectStatePtr Create();
  GLuint Program() const;
  GLint AttributePosition() const;
  void Init();
  bool Enable(const Matrix& aProjection);

protected:
  RenderObjectState();
  ~RenderObjectState();

  GLuint LoadShader(GLenum type, const char* src);

  struct State;
  State& m;
};

} // namespace vrb

#endif //VRB_RENDER_OBJECT_STATE_DOT_H
