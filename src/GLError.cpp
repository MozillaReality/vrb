#include "vrb/GLError.h"

#include <GLES2/gl2.h>

namespace vrb {

const char *
GLErrorString(GLenum aError) {
  const char *result = nullptr;
  switch (aError) {
    case GL_INVALID_ENUM:
      result = "GL_INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      result = "GL_INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      result = "GL_INVALID_OPERATION";
      break;
#if 0
    case GL_STACK_OVERFLOW:
      result = "GL_STACK_OVERFLOW";
      break;
    case GL_STACK_UNDERFLOW:
      result = "GL_STACK_UNDERFLOW";
      break;
#endif // 0
    case GL_OUT_OF_MEMORY:
      result = "GL_OUT_OF_MEMORY";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      result = "GL_INVALID_FRAMEBUFFER_OPERATION";
      break;
#if 0
    case GL_CONTEXT_LOST:
      result = "GL_CONTEXT_LOST";
      break;
    case GL_TABLE_TOO_LARGE:
      result = "GL_TABLE_TOO_LARGE";
      break;
#endif // 0
    default:
      result = "Unknown";
      break;
  }
  return result;
}

const char*
GLErrorCheck() {
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    return GLErrorString(error);
  }
  return nullptr;
}

} // namespace vrb