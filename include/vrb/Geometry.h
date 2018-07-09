/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_GEOMETRY_H
#define VRB_GEOMETRY_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/Drawable.h"
#include "vrb/Node.h"
#include "vrb/ResourceGL.h"
#include "vrb/gl.h"

#include <vector>

namespace vrb {

class Geometry : public Node, public Drawable, protected ResourceGL {
public:
  static GeometryPtr Create(CreationContextPtr& aContext);
  struct Face {
    std::vector<GLushort> vertices;
    std::vector<GLushort> uvs;
    std::vector<GLushort> normals;
  };

  // Node interface
  void Cull(CullVisitor& aVisitor, DrawableList& aDrawables) override;

  // From Drawable
  RenderStatePtr& GetRenderState() override;
  void SetRenderState(const RenderStatePtr& aRenderState) override;
  void Draw(const Camera& aCamera, const Matrix& aModelTransform) override;

  // Geometry interface
  VertexArrayPtr GetVertexArray() const;
  void SetVertexArray(const VertexArrayPtr& aVertexArray);
  void UpdateBuffers();

  void AddFace(
    const std::vector<int> &aVerticies,
    const std::vector<int> &aUVs,
    const std::vector<int> &aNormals);

  int32_t GetFaceCount() const;
  const Face& GetFace(int32_t aIndex) const;

protected:
  struct State;
  Geometry(State& aState, CreationContextPtr& aContext);
  ~Geometry();

  // From ResourceGL
  bool SupportOffRenderThreadInitialization() override;
  void InitializeGL() override;
  void ShutdownGL() override;

private:
  State& m;
  Geometry() = delete;
  VRB_NO_DEFAULTS(Geometry)
};

}

#endif //  VRB_GEOMETRY_H
