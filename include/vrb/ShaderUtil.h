#ifndef VRB_SHADER_UTIL_DOT_H
#define VRB_SHADER_UTIL_DOT_H

#include "vrb/gl.h"
#include <string>

namespace vrb {

GLint GetAttributeLocation(GLuint aProgram, const char* aName);
GLint GetAttributeLocation(GLuint aProgram, const std::string& aName);
GLint GetUniformLocation(GLuint aProgram, const char* aName);
GLint GetUniformLocation(GLuint aProgram, const std::string& aName);
GLuint LoadShader(GLenum type, const char* src);
GLuint CreateProgram (GLuint aVertexShader, GLuint aFragmentShader);

} // namespace vrb

#endif // VRB_SHADER_UTIL_DOT_H
