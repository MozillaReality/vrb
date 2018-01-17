#include "vrb/Geometry.h"

#include "vrb/private/DrawableState.h"
#include "vrb/private/NodeState.h"
#include "vrb/private/ResourceGLState.h"

#include "vrb/Camera.h"
#include "vrb/ConcreteClass.h"
#include "vrb/CullVisitor.h"
#include "vrb/DrawableList.h"
#include "vrb/GLError.h"
#include "vrb/Logger.h"
#include "vrb/Matrix.h"
#include "vrb/RenderState.h"
#include "vrb/VertexArray.h"
#include "vrb/Vector.h"

#include <GLES2/gl2.h>
#include <vector>

namespace {

struct Face {
  std::vector<GLushort> vertices;
  std::vector<GLushort> uvs;
  std::vector<GLushort> normals;
};

static void
CopyIndecies(std::vector<GLushort> &aTarget, const std::vector<int> &aSource) {
  aTarget.reserve(aSource.size());
  for (auto value: aSource) {
    if (value >= std::numeric_limits<GLushort>::max()) {
      VRB_LOG("Error, Index is greater than max size of GLushort: %d", value);
    }
    aTarget.push_back(static_cast<GLushort>(value));
  }
}

}

namespace vrb {

struct Geometry::State : public Node::State, public ResourceGL::State, public Drawable::State {
  RenderStatePtr renderState;
  VertexArrayPtr vertexArray;
  std::vector<Face> faces;
  int vertexCount;
  int triangleCount;
  GLuint vertexObjectId;
  GLuint indexObjectId;

  State() : vertexCount(0), triangleCount(0), vertexObjectId(0), indexObjectId(0) {}
};

GeometryPtr
Geometry::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<Geometry, Geometry::State> >(aContext);
}

void
Geometry::Cull(CullVisitor& aVisitor, DrawableList& aDrawables) {
  aDrawables.AddDrawable(std::move(CreateDrawablePtr()), aVisitor.GetTransform());
}

RenderStatePtr
Geometry::GetRenderState() const {
  return m.renderState;
}

void
Geometry::SetRenderState(const RenderStatePtr& aRenderState) {
  m.renderState = aRenderState;
}

VertexArrayPtr
Geometry::GetVertexArray() const {
  return m.vertexArray;
}

void
Geometry::SetVertexArray(const VertexArrayPtr& aVertexArray) {
  m.vertexArray = aVertexArray;
}

void
Geometry::AddFace(
    const std::vector<int>& aVertices,
    const std::vector<int>& aUVs,
    const std::vector<int>& aNormals) {

  Face face;
  m.vertexCount += aVertices.size();
  m.triangleCount += aVertices.size() - 2;
  CopyIndecies(face.vertices, aVertices);
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
    VRB_LOG("ERROR! Face with only %d vertices:%s", face.vertices.size(), indices.c_str());
  }
  if (aUVs.size() > 0) {
    CopyIndecies(face.uvs, aUVs);
  }

  if (aNormals.size() > 0) {
    CopyIndecies(face.normals, aNormals);
  } else if (m.vertexArray) {
    m.vertexArray->SetNormalCount(m.vertexArray->GetVertexCount());
    const Vector point = m.vertexArray->GetVertex(aVertices[0]);
    const Vector normal = ((m.vertexArray->GetVertex(aVertices[1]) - point).Cross(m.vertexArray->GetVertex(aVertices[2]) - point)).Normalize();
    for (GLuint index: face.vertices) {
      if (index <= 0) {
        VRB_LOG("Vertices index is less than zero.");
      }
      m.vertexArray->AddNormal(index - 1, normal);
      face.normals.push_back(index);
    }
  }

  m.faces.push_back(std::move(face));
}

Geometry::Geometry(State& aState, ContextWeak& aContext) :
    Node(aState, aContext),
    ResourceGL(aState, aContext),
    Drawable(aState, aContext),
    m(aState) {}
Geometry::~Geometry() {}

void
Geometry::InitializeGL() {
  VRB_CHECK(glGenBuffers(1, &m.vertexObjectId));
  VRB_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m.vertexObjectId));
  VRB_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 9 * m.triangleCount, nullptr, GL_STATIC_DRAW));
  VRB_LOG("Allocate: %d", sizeof(GLfloat) * 9 * m.triangleCount);
  std::vector<GLushort> indices;
  GLushort count = 0;
  GLintptr offset = 0;
  const GLintptr vertexSize = sizeof(GLfloat) * 3;
  for (auto& face: m.faces) {
    if (face.vertices.size() == 0) {
      break;
    }
    const GLushort vertexIndex = face.vertices[0] - 1;
    const Vector& firstVertex = m.vertexArray->GetVertex(vertexIndex);
    if (face.vertices.size() < 3) {
      std::string message;
      for (auto index: face.vertices) { message += " "; message += std::to_string(index); }
      VRB_LOG("ERROR! Face with only %d vertices:%s", face.vertices.size(), message.c_str());
      continue;
    }
    for (int ix = 1; ix <= face.vertices.size() - 2; ix++) {
      std::string out;
      VRB_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, vertexSize, firstVertex.Data()));
      indices.push_back(count);
      out += " " + firstVertex.ToString();
      count++;
      offset += vertexSize;
      const Vector v1 = m.vertexArray->GetVertex(face.vertices[ix] - 1);
      const Vector v2 = m.vertexArray->GetVertex(face.vertices[ix + 1] - 1);
      VRB_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, vertexSize, v1.Data()));
      out += "/" + v1.ToString();
      indices.push_back(count);
      count++;
      offset += vertexSize;
      VRB_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, vertexSize, v2.Data()));
      out += "/" + v2.ToString();
      indices.push_back(count);
      count++;
      offset += vertexSize;
      // VRB_LOG("array buffer[%d] o:%d c:%d : %s", ix, (int)offset, count, out.c_str());
    }
  }
  VRB_LOG("indices: %d vertex: %d", indices.size(), m.vertexCount);

  VRB_CHECK(glGenBuffers(1, &m.indexObjectId));
  VRB_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.indexObjectId));
  VRB_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), indices.data(), GL_STATIC_DRAW));
  VRB_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  VRB_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void
Geometry::ShutdownGL() {

}

void
Geometry::Draw(const Camera& aCamera, const Matrix& aModelTransform) {
  if (m.renderState->Enable(aCamera.GetPerspective(), aCamera.GetView(), aModelTransform)) {
    VRB_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m.vertexObjectId));
    VRB_CHECK(glVertexAttribPointer(m.renderState->AttributePosition(), 3, GL_FLOAT, GL_FALSE, 0, nullptr));

    VRB_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.indexObjectId));
    VRB_CHECK(glEnableVertexAttribArray(m.renderState->AttributePosition()));
    VRB_CHECK(glDrawElements(GL_TRIANGLES, m.vertexCount, GL_UNSIGNED_SHORT, 0));
    VRB_CHECK(glDisableVertexAttribArray(m.renderState->AttributePosition()));
    VRB_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    VRB_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
  }
}

}

