/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_TEXTURE_SURFACE_DOT_H
#define VRB_TEXTURE_SURFACE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/Texture.h"

#include "vrb/gl.h"
#include <string>

namespace vrb {

class TextureSurface : public Texture {
public:
  static TextureSurfacePtr Create(ContextWeak& aContext, const std::string& aName);

  void SetTextureHandle(const GLuint aHandle);

protected:
  struct State;
  TextureSurface(State& aState, ContextWeak& aContext);
  ~TextureSurface();

  // Texture interface
  void AboutToBind() override;

private:
  State& m;
  TextureSurface() = delete;
  VRB_NO_DEFAULTS(TextureSurface)
};

} // namespace vrb

#endif // VRB_TEXTURE_SURFACE_DOT_H
