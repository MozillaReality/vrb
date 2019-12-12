/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_GEOMETRY_DRAWABLE_H
#define VRB_GEOMETRY_DRAWABLE_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/Drawable.h"
#include "vrb/Node.h"

namespace vrb {

class GeometryDrawable : public Node, public Drawable {
public:
  static GeometryDrawablePtr Create(CreationContextPtr& aContext);
  // Node interface
  void Cull(CullVisitor& aVisitor, DrawableList& aDrawables) override;

  // From Drawable
  RenderStatePtr& GetRenderState() override;
  void SetRenderState(const RenderStatePtr& aRenderState) override;
  void Draw(const Camera& aCamera, const Matrix& aModelTransform) override;

  // GeometryDrawable interface
  RenderBufferPtr& GetRenderBuffer();
  void SetRenderBuffer(RenderBufferPtr& aRenderBuffer);
  void SetRenderRange(uint32_t aStartIndex, uint32_t aLength);

protected:
  struct State;
  GeometryDrawable(State& aState, CreationContextPtr& aContext);
  ~GeometryDrawable() = default;

private:
  State& m;
  GeometryDrawable() = delete;
  VRB_NO_DEFAULTS(GeometryDrawable)
};

}

#endif // VRB_GEOMETRY_DRAWABLE_H
