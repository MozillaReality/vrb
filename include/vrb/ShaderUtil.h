#ifndef VRB_SHADER_UTIL_DOT_H
#define VRB_SHADER_UTIL_DOT_H

#include "vrb/gl.h"
namespace vrb {

GLuint LoadShader(GLenum type, const char* src);
GLuint CreateProgram (GLuint aVertexShader, GLuint aFragmentShader);

} // namespace vrb

#endif // VRB_SHADER_UTIL_DOT_H
