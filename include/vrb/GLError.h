/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_GL_ERROR_DOT_H
#define VRB_GL_ERROR_DOT_H

#include <GLES2/gl2.h>
#include <android/log.h>

namespace vrb {

const char* GLErrorString(GLenum aError);
const char* GLErrorCheck();

#define VRB_CHECK(X) X;                                     \
{                                                           \
  const char* str = vrb::GLErrorCheck();                    \
  if (str) {                                                \
    __android_log_print(ANDROID_LOG_ERROR, "VRB",           \
                         "OpenGL Error: %s at%s:%s:%d",     \
                         str,                               \
                         __FILE__, __FUNCTION__, __LINE__); \
  }                                                         \
}

} // namespace vrb

#endif //  VRB_GL_ERROR_DOT_H
