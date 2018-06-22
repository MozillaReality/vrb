/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
