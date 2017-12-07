#ifndef VRB_RENDER_OBJECT_STATE_DOT_H
#define VRB_RENDER_OBJECT_STATE_DOT_H

#include <GLES2/gl2.h>
#include <memory>

namespace vrb{

class Matrix;

class RenderObjectState;
typedef std::shared_ptr<RenderObjectState> RenderObjectStatePtr;

class RenderObjectState {
public:
  static RenderObjectStatePtr Create();
  GLuint Program();
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
