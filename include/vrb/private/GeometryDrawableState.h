/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_GEOMETRY_DRAWABLE_STATE_DOT_H
#define VRB_GEOMETRY_DRAWABLE_STATE_DOT_H

#include "vrb/GeometryDrawable.h"
#include <vrb/gl.h>
#include <vrb/Logger.h>
#include "vrb/private/DrawableState.h"
#include "vrb/private/NodeState.h"
#include "vrb/RenderBuffer.h"
#include "vrb/RenderState.h"

namespace vrb {

struct GeometryDrawable::State : public Node::State, public Drawable::State {
  RenderStatePtr renderState;
  RenderBufferPtr renderBuffer;

  uint32_t rangeStart = 0;
  uint32_t rangeLength = 0;

  bool UseTexture() const {
    if (!renderState || !renderBuffer) {
      return false;
    }
    if (renderState->UVLength() != renderBuffer->UVLength()) {
      //VRB_WARN("RenderState UVLength(%d) != RenderBuffer UVLength(%d)", renderState->UVLength(),
      //         renderBuffer->UVLength());
      return false;
    }
    return renderState->HasTexture() && (renderBuffer->UVLength() > 0);
  }

  bool UseColor() const {
    if (!renderBuffer) {
      return false;
    }
    return renderBuffer->ColorLength() > 0;
  }

};

}

#endif // VRB_GEOMETRY_DRAWABLE_STATE_DOT_H
