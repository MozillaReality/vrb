/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_TEXTURE_STATE_DOT_H
#define VRB_TEXTURE_STATE_DOT_H

#include "vrb/Texture.h"

#include "vrb/gl.h"
#include <string>

namespace vrb {

struct Texture::State {
  std::string name;
  GLenum target;
  GLuint texture;

  State() : target(GL_TEXTURE_2D), texture(0) {}
};

} // namespace vrb

#endif //  VRB_TEXTURE_STATE_DOT_H
