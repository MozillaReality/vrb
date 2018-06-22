/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/ShaderUtil.h"
#include "vrb/GLError.h"
#include "vrb/Logger.h"

#include <memory>

namespace vrb {

GLint
GetAttributeLocation(GLuint aProgram, const char* aName) {
  GLint result = VRB_GL_CHECK(glGetAttribLocation(aProgram, aName));
  if (result < 0) {
    VRB_LOG("Failed to glGetAttributLocation for '%s'", aName);
  }
  return result;
}

GLint
GetAttributeLocation(GLuint aProgram, const std::string& aName) {
  return GetAttributeLocation(aProgram, aName.c_str());
}

GLint
GetUniformLocation(GLuint aProgram, const char* aName) {
  GLint result = VRB_GL_CHECK(glGetUniformLocation(aProgram, aName));
  if (result < 0) {
    VRB_LOG("Failed to glGetUniformLocation for '%s'", aName);
  }
  return result;
}

GLint
GetUniformLocation(GLuint aProgram, const std::string& aName) {
  return GetUniformLocation(aProgram, aName.c_str());
}

GLuint
LoadShader(GLenum aType, const char* aSrc) {
  GLuint shader = VRB_GL_CHECK(glCreateShader(aType));

  if (shader == 0) {
    VRB_LOG("FAILDED to create shader of type: %s", (aType == GL_VERTEX_SHADER ? "vertex shader" : "fragment shader"));
  }

  VRB_GL_CHECK(glShaderSource(shader, 1, &aSrc, nullptr));
  VRB_GL_CHECK(glCompileShader(shader));
  GLint compiled = 0;
  VRB_GL_CHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled));

  if (!compiled) {
    GLint length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (length > 1) {
      std::unique_ptr<char[]> log = std::make_unique<char[]>(length);
      VRB_GL_CHECK(glGetShaderInfoLog(shader, length, nullptr, log.get()));
      VRB_LOG("FAILED TO COMPILE SHADER:\n%s", log.get());
      VRB_LOG("From source:\n%s", aSrc);
    }
  }

  return shader;
}

GLuint
CreateProgram(GLuint aVertexShader, GLuint aFragmentShader) {
  GLuint program = VRB_GL_CHECK(glCreateProgram());
  VRB_GL_CHECK(glAttachShader(program, aVertexShader));
  VRB_GL_CHECK(glAttachShader(program, aFragmentShader));
  VRB_GL_CHECK(glLinkProgram(program));
  GLint linked = 0;
  VRB_GL_CHECK(glGetProgramiv(program, GL_LINK_STATUS, &linked));
  if (!linked) {
    GLint length = 0;
    VRB_GL_CHECK(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
    if (length > 1) {
      std::unique_ptr<char[]> log = std::make_unique<char[]>(length);
      VRB_GL_CHECK(glGetProgramInfoLog(program, length, nullptr, log.get()));
      VRB_LOG("Failed to link program:\n%s", log.get());
    }
    VRB_GL_CHECK(glDeleteProgram(program));
    program = 0;
  }
  return program;
}

} // namespace vrb
