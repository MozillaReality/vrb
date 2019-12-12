/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/RenderBuffer.h"
#include "vrb/private/ResourceGLState.h"

#include "vrb/Color.h"
#include "vrb/ConcreteClass.h"
#include "vrb/Logger.h"
#include "vrb/GLError.h"
#include "vrb/Vector.h"

#include "vrb/gl.h"

namespace vrb {

struct RenderBuffer::State {
  GLsizei vertexCount = 0;
  GLsizei indexCount = 0;
  GLuint vertexObjectId = 0;
  GLuint indexObjectId = 0;
  size_t positionOffset = 0;
  GLsizei positionLength = 0;
  size_t normalOffset = 0;
  GLsizei normalLength = 0;
  size_t uvOffset = 0;
  GLsizei uvLength = 0;
  size_t colorOffset = 0;
  GLsizei colorLength = 0;

  State() = default;
  ~State() = default;
  GLsizei PositionSize() const {
    return positionLength * sizeof(float);
  }

  GLsizei NormalSize() const {
    return normalLength * sizeof(float);
  }

  GLsizei UVSize() const {
    return uvLength * sizeof(float);
  }
  GLsizei ColorSize() const {
    return colorLength* sizeof(float);
  }

  GLsizei VertexSize() const {
    return PositionSize() + NormalSize() + ColorSize() + UVSize();
  }
};

RenderBufferPtr
RenderBuffer::Create(CreationContextPtr& aContext) {
  return std::make_shared<ConcreteClass<RenderBuffer, RenderBuffer::State>>(aContext);
}

void
RenderBuffer::SetVertexObject(GLuint aObject, const GLsizei aCount) {
  m.vertexObjectId = aObject;
  m.vertexCount = aCount;
}

GLuint
RenderBuffer::GetVertexObject() const {
  return m.vertexObjectId;
}

void
RenderBuffer::SetIndexObject(GLuint aObject, const GLsizei aCount) {
  m.indexObjectId = aObject;
  m.indexCount = aCount;
}

GLuint
RenderBuffer::GetIndexObject() const {
  return m.indexObjectId;
}

GLsizei
RenderBuffer::VertexCount() const {
  return m.vertexCount;
}

GLsizei
RenderBuffer::VertexSize() const {
  return m.VertexSize();
}

GLsizei
RenderBuffer::IndexCount() const {
  return m.indexCount;
}
void
RenderBuffer::DefinePosition(const size_t aOffset, const GLsizei aLength) {
  m.positionOffset = aOffset;
  m.positionLength = aLength;
}

size_t
RenderBuffer::PositionOffset() const {
  return m.positionOffset;
}

GLsizei
RenderBuffer::PositionLength() const {
  return m.positionLength;
}

GLsizei
RenderBuffer::PositionSize() const {
  return m.PositionSize();
}

void
RenderBuffer::DefineNormal(const size_t aOffset, const GLsizei aLength) {
  m.normalOffset = aOffset;
  m.normalLength = aLength;
}

size_t
RenderBuffer::NormalOffset() const {
  return m.normalOffset;
}

GLsizei
RenderBuffer::NormalLength() const {
  return m.normalLength;
}

GLsizei
RenderBuffer::NormalSize() const {
  return m.NormalSize();
}

void
RenderBuffer::DefineUV(const size_t aOffset, const GLsizei aLength) {
  m.uvOffset = aOffset;
  m.uvLength = aLength;
}

size_t
RenderBuffer::UVOffset() const {
  return m.uvOffset;
}

GLsizei
RenderBuffer::UVLength() const {
  return m.uvLength;
}

GLsizei
RenderBuffer::UVSize() const {
  return m.UVSize();
}

void
RenderBuffer::DefineColor(const size_t aOffset, const GLsizei aLength) {
  m.colorOffset = aOffset;
  m.colorLength = aLength;
}

size_t
RenderBuffer::ColorOffset() const {
  return m.colorOffset;
}

GLsizei
RenderBuffer::ColorLength() const {
  return m.colorLength;
}

GLsizei
RenderBuffer::ColorSize() const {
  return m.ColorSize();
}

void
RenderBuffer::Bind() {
  VRB_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m.vertexObjectId));
  VRB_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.indexObjectId));
}

void
RenderBuffer::Unbind() {
  VRB_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  VRB_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

RenderBuffer::RenderBuffer(State& aState, CreationContextPtr& aContext): m(aState) {}

} // namespace vrb
