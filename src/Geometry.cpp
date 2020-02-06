/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/Geometry.h"

#include "vrb/private/GeometryDrawableState.h"
#include "vrb/private/ResourceGLState.h"

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

#include <limits>
#include <vector>

namespace {

void
CopyIndices(std::vector<GLushort> &aTarget, const std::vector<int> &aSource) {
  aTarget.reserve(aSource.size());
  for (auto value: aSource) {
    if (value >= std::numeric_limits<GLushort>::max()) {
      VRB_ERROR("Index is greater than max size of GLushort: %d", value);
    }
    aTarget.push_back(static_cast<GLushort>(value));
  }
}

}

namespace vrb {

struct Geometry::State : public GeometryDrawable::State, public ResourceGL::State {
  VertexArrayPtr vertexArray;
  std::vector<Face> faces;
  GLsizei vertexCount = 0;
  GLsizei triangleCount = 0;

  State() = default;
  ~State() = default;
};

GeometryPtr
Geometry::Create(CreationContextPtr& aContext) {
  return std::make_shared<ConcreteClass<Geometry, Geometry::State> >(aContext);
}

// Geometry interface
VertexArrayPtr
Geometry::GetVertexArray() const {
  return m.vertexArray;
}

void
Geometry::SetVertexArray(const VertexArrayPtr& aVertexArray) {
  m.vertexArray = aVertexArray;
}

void
Geometry::UpdateBuffers() {
  GLuint vertexObjectId = m.renderBuffer->GetVertexObject();
  GLuint indexObjectId = m.renderBuffer->GetIndexObject();
  if (vertexObjectId == 0 || indexObjectId == 0) {
    VRB_WARN("Geometry GL objects not created");
    return;
  }

  const bool kHasTextureCoords = m.vertexArray->GetUVCount() > 0;
  const bool kHasColor = m.vertexArray->GetColorCount() > 0;
  const GLsizei kPositionSize = m.renderBuffer->PositionSize();
  const GLsizei kNormalSize = m.renderBuffer->NormalSize();
  const GLsizei kUVSize = m.renderBuffer->UVSize();
  const GLsizei kColorSize = m.renderBuffer->ColorSize();

  VRB_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexObjectId));

  std::vector<GLushort> indices;
  GLushort count = 0;
  GLintptr offset = 0;

  for (auto& face: m.faces) {
    if (face.vertices.empty()) {
      break;
    }
    if (face.vertices.size() < 3) {
      std::string message;
      for (auto index: face.vertices) { message += " "; message += std::to_string(index); }
      VRB_ERROR("Face with only %d vertices:%s", (int32_t)face.vertices.size(), message.c_str());
      continue;
    }
    const auto vertexIndex = (GLushort)(face.vertices[0] - 1);
    const auto normalIndex = (GLushort)(face.normals[0] - 1);
    const auto uvIndex = (GLushort)(kHasTextureCoords ? face.uvs[0] - 1 : -1);
    const Vector& firstVertex = m.vertexArray->GetVertex(vertexIndex);
    const Vector& firstNormal = m.vertexArray->GetNormal(normalIndex);
    const Vector& firstUV = m.vertexArray->GetUV(uvIndex);
    const Color& firstColor = m.vertexArray->GetColor(vertexIndex);
    for (int ix = 1; ix <= face.vertices.size() - 2; ix++) {
      VRB_GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, kPositionSize, firstVertex.Data()));
      offset += kPositionSize;
      VRB_GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, kNormalSize, firstNormal.Data()));
      offset += kNormalSize;
      if (kHasTextureCoords) {
        VRB_GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, kUVSize, firstUV.Data()));
        offset += kUVSize;
      }
      if (kHasColor) {
        VRB_GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, kColorSize, firstColor.Data()));
        offset += kColorSize;
      }
      indices.push_back(count);
      count++;

      const Vector v1 = m.vertexArray->GetVertex(face.vertices[ix] - 1);
      const Vector n1 = m.vertexArray->GetNormal(face.normals[ix] - 1);
      VRB_GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, kPositionSize, v1.Data()));
      offset += kPositionSize;
      VRB_GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, kNormalSize, n1.Data()));
      offset += kNormalSize;
      if (kHasTextureCoords) {
        const Vector uv1 = m.vertexArray->GetUV(face.uvs[ix] - 1);
        VRB_GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, kUVSize, uv1.Data()));
        offset += kUVSize;
      }
      if (kHasColor) {
        const Color& color1 = m.vertexArray->GetColor(face.vertices[ix] - 1);
        VRB_GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, kColorSize, color1.Data()));
        offset += kColorSize;
      }
      indices.push_back(count);
      count++;

      const Vector v2 = m.vertexArray->GetVertex(face.vertices[ix + 1] - 1);
      const Vector n2 = m.vertexArray->GetNormal(face.normals[ix + 1] - 1);
      VRB_GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, kPositionSize, v2.Data()));
      offset += kPositionSize;
      VRB_GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, kNormalSize, n2.Data()));
      offset += kNormalSize;
      if (kHasTextureCoords) {
        const Vector uv2 = m.vertexArray->GetUV(face.uvs[ix + 1] - 1);
        VRB_GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, kUVSize, uv2.Data()));
        offset += kUVSize;
      }
      if (kHasColor) {
        const Color& color2 = m.vertexArray->GetColor(face.vertices[ix + 1] - 1);
        VRB_GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, kColorSize, color2.Data()));
        offset += kColorSize;
      }
      indices.push_back(count);
      count++;
    }
  }

  VRB_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexObjectId));
  VRB_GL_CHECK(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLushort) * indices.size(), indices.data()));


  VRB_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  VRB_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}


void
Geometry::AddFace(
    const std::vector<int>& aVertices,
    const std::vector<int>& aUVs,
    const std::vector<int>& aNormals) {

  Face face;
  m.vertexCount += aVertices.size();
  m.triangleCount += aVertices.size() - 2;
  CopyIndices(face.vertices, aVertices);
  if (face.vertices.size() < 3) {
    std::string indices;
    for (auto ix: face.vertices) {
      indices += " ";
      indices += std::to_string(ix);
    }
    indices += " from:";
    for (auto ix: aVertices) {
      indices += " ";
      indices += std::to_string(ix);
    }
    VRB_ERROR("Face with only %d vertices:%s", (int)face.vertices.size(), indices.c_str());
  }
  if (!aUVs.empty()) {
    CopyIndices(face.uvs, aUVs);
  }

  if (!aNormals.empty() && (aNormals[0] != 0)) {
    CopyIndices(face.normals, aNormals);
  } else if (m.vertexArray) {
    m.vertexArray->SetNormalCount(m.vertexArray->GetVertexCount());
    const Vector point = m.vertexArray->GetVertex(aVertices[0] - 1);
    const Vector normal = ((m.vertexArray->GetVertex(aVertices[1] - 1) - point).Cross(m.vertexArray->GetVertex(aVertices[2] - 1) - point)).Normalize();
    m.vertexArray->AppendNormal(normal);
    for (GLuint index: face.vertices) {
      if (index <= 0) {
        VRB_ERROR("Vertices index is less than zero.");
      }
      if (normal.Magnitude() > FLT_EPSILON) {
        m.vertexArray->AddNormal(index - 1, normal);
      }
      face.normals.push_back(index);
    }
  }

  m.faces.push_back(std::move(face));
}

int32_t
Geometry::GetFaceCount() const {
  return m.faces.size();
}

const Geometry::Face&
Geometry::GetFace(int32_t aIndex) const {
  return m.faces[aIndex];
}

Geometry::Geometry(State& aState, CreationContextPtr& aContext) :
    GeometryDrawable(aState, aContext),
    ResourceGL(aState, aContext),
    m(aState)
{
  m.renderBuffer = RenderBuffer::Create(aContext);
}

Geometry::~Geometry() {}

// ResourceGL interface
bool
Geometry::SupportOffRenderThreadInitialization() {
  return true;
}

void
Geometry::InitializeGL() {
  if (!m.vertexArray) {
    VRB_ERROR("Unable to initialize Geometry Node. No VertexArray set");
  }

  size_t definedOffset = 0;
  m.renderBuffer->DefinePosition(definedOffset);
  definedOffset = m.renderBuffer->PositionOffset() + m.renderBuffer->PositionSize();
  m.renderBuffer->DefineNormal(definedOffset);
  definedOffset = m.renderBuffer->NormalOffset() + m.renderBuffer->NormalSize();
  if (m.vertexArray->GetUVCount() > 0) {
    m.renderBuffer->DefineUV(definedOffset, m.vertexArray->GetUVLength());
    definedOffset = m.renderBuffer->UVOffset() + m.renderBuffer->UVSize();
  }
  if (m.vertexArray->GetColorCount() > 0) {
    m.renderBuffer->DefineColor(definedOffset);
  }
  GLuint vertexObjectId = 0;
  GLuint indexObjectId = 0;
  VRB_GL_CHECK(glGenBuffers(1, &vertexObjectId));
  VRB_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexObjectId));
  GLsizei kTriangleSize = m.renderBuffer->VertexSize() * 3;

  VRB_GL_CHECK(glBufferData(GL_ARRAY_BUFFER, kTriangleSize * m.triangleCount, nullptr, GL_STATIC_DRAW));
  VRB_LOG("Allocate: %d for GL_ARRAY_BUFFER: %d", kTriangleSize * m.triangleCount, vertexObjectId);

  VRB_GL_CHECK(glGenBuffers(1, &indexObjectId));
  VRB_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexObjectId));
  VRB_GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * m.triangleCount * 3, nullptr, GL_STATIC_DRAW));
  VRB_LOG("Allocate: %d for GL_ELEMENT_ARRAY_BUFFER: %d", (int32_t)sizeof(GLushort) * m.triangleCount * 3, indexObjectId);
  m.renderBuffer->SetVertexObject(vertexObjectId, m.vertexCount);
  m.renderBuffer->SetIndexObject(indexObjectId, m.triangleCount * 3);

  UpdateBuffers();
}

void
Geometry::ShutdownGL() {

}

}

