#include "vrb/RenderObject.h"
#include "vrb/Vector.h"

#include <vector>
#include <iostream>

#if defined(ANDROID)
#include <android/log.h>
#define VRLOG(format, ...) __android_log_print(ANDROID_LOG_INFO, "VRBrowser", format, ##__VA_ARGS__);
#else
#define VRLOG(...);
#endif

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
  std::vector<int> vertices;
  std::vector<int> uvs;
  std::vector<int> normals;
};

struct Group {
  std::vector<std::string> names;
  std::vector<Face> faces;
};

class RenderObjectAlloc : public RenderObject {
public:
  RenderObjectAlloc(const std::string &aName) : RenderObject(aName) {}
  ~RenderObjectAlloc() {};
};

}

namespace vrb {

struct RenderObject::State {
  std::string name;
  std::vector<Group> groups;
  std::vector<Vector> vertices;
  std::vector<Vector> uvs;
  std::vector<Vector> normals;
};

RenderObjectPtr
RenderObject::Create(const std::string &aName) {
  return std::make_shared<RenderObjectAlloc>(aName);
}

RenderObject::RenderObject(const std::string& aName) : m(*(new State)) {
  m.name = aName;
}

RenderObject::~RenderObject() {
  VRLOG("DESTROY RenderObject:%p", (void*)this);
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
  m.vertices.push_back(aVertex);
  return m.vertices.size();
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
  face.vertices = aVerticies;
  face.uvs = aUVs;
  face.normals = aNormals;

  return result;
}

void
RenderObject::Dump() {
  std::cout<<"RenderObject: " << m.name << std::endl;
  VRLOG("RenderObject: '%s'", m.name.c_str());
  std::cout << "  Vertices:" << (m.vertices.size() == 0 ? " None" : "") << std::endl;
  VRLOG("  Vertices[%d]:", m.vertices.size());
  std::string list;
  for (auto vertex: m.vertices) {
    std::cout << "    (" << vertex.x() << ", " << vertex.y() << ", " << vertex.z() << ")" << std::endl;
  }
  std::cout << "  UVs:" << (m.uvs.size() == 0 ? " None" : "") << std::endl;
  VRLOG("  UVs[%d]:", m.uvs.size());
  for (auto uv: m.uvs) {
    std::cout << "    (" << uv.x() << ", " << uv.y() << ", " << uv.z() << ")" << std::endl;
  }
  std::cout << "  Normals:" << (m.normals.size() == 0 ? " None" : "") << std::endl;
  VRLOG("  Normals[%d]:", m.normals.size());
  for (auto normal: m.normals) {
    std::cout << "    (" << normal.x() << ", " << normal.y() << ", " << normal.z() << ")" << std::endl;
  }
  for (auto group: m.groups) {
    std::cout << "  Group:";
    std::string nameStr("");
    for (auto name: group.names) {
      std::cout << " '" << name << "'";
      nameStr += " '";
      nameStr += name;
      nameStr += "'";
    }
    std::cout << std::endl;
    VRLOG("  Group[%d]: %s", group.faces.size(), nameStr.c_str());
    for (auto face: group.faces) {
      std::cout << "    f:";
      const size_t size = face.vertices.size();
      for (size_t ix = 0; ix < size; ix++) {
        std::cout << " " << face.vertices[ix] << "/" << face.uvs[ix] << "/" << face.normals[ix];
      }
      std::cout << std::endl;
    }
  }
}

} // namespace vrb
