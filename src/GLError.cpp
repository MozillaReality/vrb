/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/GLError.h"

#include "vrb/gl.h"

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
