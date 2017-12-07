#ifndef VRB_GL_ERROR_DOT_H
#define VRB_GL_ERROR_DOT_H

#include <GLES2/gl2.h>
#include <android/log.h>

namespace vrb {

const char* GLErrorString(GLenum aError);
const char* GLErrorCheck();

#define VRB_CHECK(X) X;                                     \
{                                                           \
  const char* str = GLErrorCheck();                         \
  if (str) {                                                \
    __android_log_print(ANDROID_LOG_ERROR, "VRBrowser",     \
                         "OpenGL Error: %s at%s:%s:%d",     \
                         str,                               \
                         __FILE__, __FUNCTION__, __LINE__); \
  }                                                         \
}

} // namespace vrb

#endif //  VRB_GL_ERROR_DOT_H