/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/GeometryDrawable.h"

#include "vrb/private/GeometryDrawableState.h"

#include "vrb/Camera.h"
#include "vrb/Color.h"
#include "vrb/ConcreteClass.h"
#include "vrb/CullVisitor.h"
#include "vrb/DrawableList.h"
#include "vrb/GLError.h"
#include "vrb/Logger.h"
#include "vrb/Matrix.h"
#include "vrb/RenderBuffer.h"
#include "vrb/RenderState.h"
#include "vrb/Texture.h"
#include "vrb/VertexArray.h"
#include "vrb/Vector.h"

#include <vector>

namespace vrb {

GeometryDrawablePtr
GeometryDrawable::Create(CreationContextPtr& aContext) {
  return std::make_shared<ConcreteClass<GeometryDrawable, GeometryDrawable::State> >(aContext);
}

// Node interface
void
GeometryDrawable::Cull(CullVisitor& aVisitor, DrawableList& aDrawables) {
  aDrawables.AddDrawable(std::move(CreateDrawablePtr()), aVisitor.GetTransform());
}

// Drawable interface
RenderStatePtr&
GeometryDrawable::GetRenderState() {
  return m.renderState;
}

void
GeometryDrawable::SetRenderState(const RenderStatePtr& aRenderState) {
  m.renderState = aRenderState;
}

void
GeometryDrawable::Draw(const Camera& aCamera, const Matrix& aModelTransform) {
  if (m.renderState->Enable(aCamera.GetPerspective(), aCamera.GetView(), aModelTransform)) {
    const bool kUseTexture = m.UseTexture();
    const bool kUseColor = m.UseColor();
    const GLsizei kSize = m.renderBuffer->VertexSize();
    m.renderBuffer->Bind();

    VRB_GL_CHECK(glVertexAttribPointer((GLuint)m.renderState->AttributePosition(), m.renderBuffer->PositionLength(), GL_FLOAT, GL_FALSE, kSize, (const GLvoid*)m.renderBuffer->PositionOffset()));
    VRB_GL_CHECK(glVertexAttribPointer((GLuint)m.renderState->AttributeNormal(), m.renderBuffer->NormalLength(), GL_FLOAT, GL_FALSE, kSize, (const GLvoid*)m.renderBuffer->NormalOffset()));
    if (kUseTexture) {
      VRB_GL_CHECK(glVertexAttribPointer((GLuint)m.renderState->AttributeUV(), m.renderBuffer->UVLength(), GL_FLOAT, GL_FALSE, kSize, (const GLvoid*)m.renderBuffer->UVOffset()));
    }
    if (kUseColor) {
      VRB_GL_CHECK(glVertexAttribPointer((GLuint)m.renderState->AttributeColor(), m.renderBuffer->ColorLength(), GL_FLOAT, GL_FALSE, kSize, (const GLvoid*)m.renderBuffer->ColorOffset()));
    }

    VRB_GL_CHECK(glEnableVertexAttribArray((GLuint)m.renderState->AttributePosition()));
    VRB_GL_CHECK(glEnableVertexAttribArray((GLuint)m.renderState->AttributeNormal()));
    if (kUseTexture) {
      VRB_GL_CHECK(glEnableVertexAttribArray((GLuint)m.renderState->AttributeUV()));
    }
    if (kUseColor) {
      VRB_GL_CHECK(glEnableVertexAttribArray((GLuint)m.renderState->AttributeColor()));
    }
    const int32_t maxLength = m.renderBuffer->IndexCount();
    if (m.rangeLength == 0) {
      VRB_GL_CHECK(glDrawElements(GL_TRIANGLES, maxLength, GL_UNSIGNED_SHORT, 0));
    } else if ((m.rangeStart + m.rangeLength) <= maxLength) {
      VRB_GL_CHECK(glDrawElements(GL_TRIANGLES, m.rangeLength, GL_UNSIGNED_SHORT, (void*)(m.rangeStart * sizeof(GLushort))));
    } else {
      VRB_WARN("Invalid geometry range (%u-%u). Max geometry length %d", m.rangeStart, m.rangeLength + m.rangeLength, maxLength);
    }
    VRB_GL_CHECK(glDisableVertexAttribArray((GLuint)m.renderState->AttributePosition()));
    VRB_GL_CHECK(glDisableVertexAttribArray((GLuint)m.renderState->AttributeNormal()));
    if (kUseTexture) {
      VRB_GL_CHECK(glDisableVertexAttribArray((GLuint)m.renderState->AttributeUV()));
    }
    if (kUseColor) {
      VRB_GL_CHECK(glDisableVertexAttribArray((GLuint)m.renderState->AttributeColor()));
    }
    m.renderBuffer->Unbind();
    m.renderState->Disable();
  }
}

// GeometryDrawable interface
RenderBufferPtr&
GeometryDrawable::GetRenderBuffer() {
  return m.renderBuffer;
}

void
GeometryDrawable::SetRenderBuffer(RenderBufferPtr& aRenderBuffer) {
  m.renderBuffer = aRenderBuffer;
}

void
GeometryDrawable::SetRenderRange(uint32_t aStartIndex, uint32_t aLength) {
  m.rangeStart = aStartIndex;
  m.rangeLength = aLength;
}

GeometryDrawable::GeometryDrawable(State& aState, CreationContextPtr& aContext) :
    Node(aState, aContext),
    Drawable(aState, aContext),
    m(aState)
{
  m.renderBuffer = RenderBuffer::Create(aContext);
}

} // vrb

