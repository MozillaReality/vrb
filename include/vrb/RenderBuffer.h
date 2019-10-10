/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_RENDER_BUFFER_DOT_H
#define VRB_RENDER_BUFFER_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include "vrb/gl.h"

namespace vrb {

class RenderBuffer {
public:
  static RenderBufferPtr Create(CreationContextPtr& aContext);
  void SetVertexObject(GLuint aObject, GLsizei aCount);
  GLuint GetVertexObject() const;
  void SetIndexObject(GLuint aObject, const GLsizei aCount);
  GLuint GetIndexObject() const;
  GLsizei VertexCount() const;
  GLsizei VertexSize() const;
  GLsizei IndexCount() const;
  void DefinePosition(const size_t aOffset, const GLsizei aLength = 3);
  size_t PositionOffset() const;
  GLsizei PositionSize() const;
  GLsizei PositionLength() const;
  void DefineNormal(const size_t aOffset, const GLsizei aLength = 3);
  size_t NormalOffset() const;
  GLsizei NormalSize() const;
  GLsizei NormalLength() const;
  void DefineUV(const size_t aOffset, const GLsizei aLength = 2);
  size_t UVOffset() const;
  GLsizei UVSize() const;
  GLsizei UVLength() const;
  void DefineColor(const size_t aOffset, const GLsizei aLength = 4);
  size_t ColorOffset() const;
  GLsizei ColorSize() const;
  GLsizei ColorLength() const;
  void Bind();
  void Unbind();

protected:
  struct State;
  RenderBuffer(State& aState, CreationContextPtr& aContext);
  ~RenderBuffer() = default;

private:
  State& m;
  RenderBuffer() = delete;
  VRB_NO_DEFAULTS(RenderBuffer)
};

} // namespace vrb

#endif // VRB_RENDER_BUFFER_DOT_H
