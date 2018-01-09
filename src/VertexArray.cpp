#include "vrb/VertexArray.h"

#include "vrb/ConcreteClass.h"
#include "vrb/Vector.h"

#include <vector>

namespace vrb {

static const vrb::Vector cZeroVector;

struct VertexArray::State {
  std::vector<Vector> vertices;
  std::vector<Vector> normals;
  std::vector<Vector> uvs;
};

VertexArrayPtr
VertexArray::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<VertexArray, VertexArray::State> >(aContext);
}

int
VertexArray::GetVertexCount() const {
  return m.vertices.size();
}

int
VertexArray::GetNormalCount() const {
  return m.normals.size();
}

int
VertexArray::GetUVCount() const {
  return m.uvs.size();
}

const Vector&
VertexArray::GetVertex(const int aIndex) const {
  if (aIndex >= m.vertices.size()) {
    return cZeroVector;
  }
  return m.vertices[aIndex];
}

const Vector&
VertexArray::GetNormal(const int aIndex) const {
  if (aIndex >= m.normals.size()) {
    return cZeroVector;
  }
  return m.normals[aIndex];
}

const Vector&
VertexArray::GetUV(const int aIndex) const {
  if (aIndex >= m.uvs.size()) {
    return cZeroVector;
  }
  return m.uvs[aIndex];
}

void
VertexArray::SetVertex(const int aIndex, const Vector& aPoint) {
  m.vertices.resize(aIndex + 1);
  m.vertices[aIndex] = aPoint;
}

void
VertexArray::SetNormal(const int aIndex, const Vector& aNormal) {
  m.normals.resize(aIndex + 1);
  m.normals[aIndex] = aNormal;
}

void
VertexArray::SetUV(const int aIndex, const Vector& aUV) {
  m.uvs.resize(aIndex + 1);
  m.uvs[aIndex] = aUV;
}

int
VertexArray::AddVertex(const Vector& aPoint) {
  m.vertices.push_back(aPoint);
  return m.vertices.size() - 1;
}

int
VertexArray::AddNormal(const Vector& aNormal) {
  m.normals.push_back(aNormal);
  return m.normals.size() - 1;
}

int
VertexArray::AddUV(const Vector& aUV) {
  m.uvs.push_back(aUV);
  return m.uvs.size() - 1;
}

VertexArray::VertexArray(State& aState, ContextWeak& aContext) : m(aState) {}
VertexArray::~VertexArray() {}

}

