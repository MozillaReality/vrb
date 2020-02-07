/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_FBO_DOT_H
#define VRB_FBO_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include "vrb/gl.h"

namespace vrb {

class FBO {
public:
  struct Attributes {
    Attributes();
    Attributes(bool aDepth, bool aMultiview, int aSamples);
    bool depth;
    bool multiview;
    int samples;
  };
  static FBOPtr Create(RenderContextPtr& aContext);
  bool IsValid() const;
  void SetTextureHandle(const GLuint aHandle,
                        const int32_t aWidth,
                        const int32_t aHeight,
                        const FBO::Attributes& aAttributes = {});
  void Bind(GLenum target = GL_FRAMEBUFFER);
  void Unbind();
  const FBO::Attributes& GetAttributes() const;
  GLuint GetHandle() const;
protected:
  struct State;
  FBO(State& aState);
  ~FBO();
private:
  State& m;
  FBO() = delete;
  VRB_NO_DEFAULTS(FBO)
};

}
#endif // VRB_FBO_DOT_H
