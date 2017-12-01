#include "vrb/RenderObject.h"
#include "vrb/Vector.h"

#include <vector>
#include <iostream>

using namespace vrb;

namespace {

struct Vertex {
  Vector* point;
  Vector* uv;
  Vector* normal;

  Vertex()
      : point(nullptr)
      , uv(nullptr)
      , normal(nullptr) {}
};

struct Face {
  std::vector<Vertex> map;
  std::vector<int> verticies;
  std::vector<int> uvs;
  std::vector<int> normals;
};

struct Group {
  std::vector<std::string> names;
  std::vector<Face> faces;
};

}

namespace vrb {

struct RenderObject::State {
  std::string name;
  std::vector<Group> groups;
  std::vector<Vector> verticies;
  std::vector<Vector> uvs;
  std::vector<Vector> normals;
};

RenderObject::RenderObject() : m(*(new State)) {

}

RenderObject::RenderObject(const std::string& aName) : m(*(new State)) {
  m.name = aName;
}

RenderObject::~RenderObject() {
  delete &m;
}

std::string
RenderObject::GetName() const {
  return m.name;
}

void
RenderObject::Draw() {

}

int
RenderObject::AddVertex(const Vector& aVertex) {
  m.verticies.push_back(aVertex);
  return m.verticies.size();
}

int
RenderObject::AddNormal(const Vector& aNormal) {
  m.normals.push_back(aNormal);
  return m.normals.size();
}

int
RenderObject::AddUV(const Vector& aUV) {
  m.uvs.push_back(aUV);
  return m.uvs.size();
}

int
RenderObject::CreateGroup(const std::vector<std::string>& aNames) {
  Group group;
  group.names = aNames;
  m.groups.push_back(group);
  return m.groups.size();
}

int
RenderObject::AddFace(
      const int aGroup,
      const std::vector<int>& aVerticies,
      const std::vector<int>& aUVs,
      const std::vector<int>& aNormals) {
  const int index = aGroup - 1;
  if (index >= m.groups.size()) {
    return -1;
  }

  Group& group = m.groups[index];
  group.faces.push_back(Face());
  const int result = group.faces.size();
  const int faceIndex = result - 1;
  Face& face = group.faces[faceIndex];
  face.verticies = aVerticies;
  face.uvs = aUVs;
  face.normals = aNormals;

  return result;
}

void
RenderObject::Dump() {
  std::cout<<"RenderObject: " << m.name << std::endl;
  std::cout << "  Verticies:" << (m.verticies.size() == 0 ? " None" : "") << std::endl;
  for (auto vertex: m.verticies) {
    std::cout << "    (" << vertex.x() << ", " << vertex.y() << ", " << vertex.z() << ")" << std::endl;
  }
  std::cout << "  UVs:" << (m.uvs.size() == 0 ? " None" : "") << std::endl;
  for (auto uv: m.uvs) {
    std::cout << "    (" << uv.x() << ", " << uv.y() << ", " << uv.z() << ")" << std::endl;
  }
  std::cout << "  Normals:" << (m.normals.size() == 0 ? " None" : "") << std::endl;
  for (auto normal: m.normals) {
    std::cout << "    (" << normal.x() << ", " << normal.y() << ", " << normal.z() << ")" << std::endl;
  }
  for (auto group: m.groups) {
    std::cout << "  Group:";
    for (auto name: group.names) {
      std::cout << " '" << name << "'";
    }
    std::cout << std::endl;
    for (auto face: group.faces) {
      std::cout << "    f:";
      const size_t size = face.verticies.size();
      for (size_t ix = 0; ix < size; ix++) {
         std::cout << " " << face.verticies[ix] << "/" << face.uvs[ix] << "/" << face.normals[ix];
      }
      std::cout << std::endl;
    }
  }
}

} // namespace vrb
