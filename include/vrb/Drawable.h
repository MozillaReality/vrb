/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_DRAWABLE_DOT_H
#define VRB_DRAWABLE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/Node.h"

namespace vrb {

class Drawable : public std::enable_shared_from_this<Drawable> {
public:
  DrawablePtr CreateDrawablePtr();
  virtual RenderStatePtr& GetRenderState() = 0;
  virtual void SetRenderState(const RenderStatePtr& aRenderState) = 0;
  virtual void Draw(const Camera& aCamera, const Matrix& aModelTransform) = 0;
protected:
  struct State;
  Drawable(State& aState, CreationContextPtr& aContext);
  ~Drawable();
private:
  State& m;
  Drawable() = delete;
  VRB_NO_DEFAULTS(Drawable)
};

}

#endif // VRB_DRAWABLE_DOT_H
