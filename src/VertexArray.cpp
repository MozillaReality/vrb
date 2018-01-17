#include "vrb/VertexArray.h"

#include "vrb/ConcreteClass.h"
#include "vrb/Vector.h"

#include <vector>

namespace vrb {

static const vrb::Vector cZeroVector;

struct VertexArray::State {
  struct NormalState {
    Vector normal;
    float count;
    NormalState() : count(0.0f) {}
    NormalState(const Vector& aNormal) : normal(aNormal), count(1.0f) {}
  };
  std::vector<Vector> vertices;
  std::vector<NormalState> normals;
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

void
VertexArray::SetNormalCount(const int aCount) {
  m.normals.resize(aCount);
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
  return m.normals[aIndex].normal;
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
VertexArray::AppendVertex(const Vector& aPoint) {
  m.vertices.push_back(aPoint);
  return m.vertices.size() - 1;
}

int
VertexArray::AppendNormal(const Vector& aNormal) {
  m.normals.push_back(State::NormalState(aNormal));
  return m.normals.size() - 1;
}

void
VertexArray::AddNormal(const int aIndex, const Vector& aNormal) {
  m.normals.resize(aIndex + 1);
  State::NormalState& ns = m.normals[aIndex];
  const float originalCount = ns.count;
  ns.count++;
  ns.normal = ((ns.normal * originalCount) + aNormal) / ns.count;
}

int
VertexArray::AppendUV(const Vector& aUV) {
  m.uvs.push_back(aUV);
  return m.uvs.size() - 1;
}

VertexArray::VertexArray(State& aState, ContextWeak& aContext) : m(aState) {}
VertexArray::~VertexArray() {}

}

