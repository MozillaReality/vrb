#include "vrb/RenderObject.h"
#include "vrb/GLError.h"
#include "vrb/Matrix.h"
#include "vrb/RenderObjectState.h"
#include "vrb/Vector.h"

#include <GLES2/gl2.h>
#include <vector>
#include <iostream>

#if defined(ANDROID)

#include <android/log.h>

#define VRLOG(format, ...) __android_log_print(ANDROID_LOG_INFO, "VRBrowser", format, ##__VA_ARGS__);
#define VRLINE VRLOG("%s:%s:%d", __FILE__, __FUNCTION__, __LINE__)
#else
#define VRLOG(...);
#define VRLINE;
#endif

using namespace vrb;

namespace {

struct Vertex {
  Vector *point;
  Vector *uv;
  Vector *normal;

  Vertex()
      : point(nullptr), uv(nullptr), normal(nullptr) {}
};

struct Face {
  std::vector<Vertex> map;
  std::vector<GLushort> vertices;
  std::vector<GLushort> uvs;
  std::vector<GLushort> normals;
};

struct Group {
  std::vector<std::string> names;
  std::vector<Face> faces;
  int vertexCount;
  GLuint vertexObjectId;
  GLuint indexObjectId;

  Group()
      : vertexCount(0), vertexObjectId(0), indexObjectId(0) {}
};

class RenderObjectAlloc : public RenderObject {
public:
  RenderObjectAlloc(const std::string &aName) : RenderObject(aName) {}
};

static void
CopyIndecies(std::vector<GLushort> &aTarget, const std::vector<int> &aSource) {
  aTarget.reserve(aSource.size());
  for (auto value: aSource) {
    if (value >= std::numeric_limits<GLushort>::max()) {
      VRLOG("Error, Index is greater than max size of GLushort: %d", value);
    }
    aTarget.push_back(static_cast<GLushort>(value));
  }
}

}

namespace vrb {

struct RenderObject::State {
  std::string name;
  std::vector<Group> groups;
  std::vector<Vector> vertices;
  std::vector<Vector> uvs;
  std::vector<Vector> normals;
  RenderObjectStatePtr renderState;
};

RenderObjectPtr
RenderObject::Create(const std::string &aName) {
  return std::make_shared<RenderObjectAlloc>(aName);
}

RenderObject::RenderObject(const std::string &aName) : m(*(new State)) {
  m.name = aName;
  m.renderState = RenderObjectState::Create();
}

RenderObject::~RenderObject() {
  VRLOG("DESTROY RenderObject:%p", (void *) this);
  delete &m;
}

std::string
RenderObject::GetName() const {
  return m.name;
}

static GLint sPOS;

void
RenderObject::Init() {
  VRLINE;
  m.renderState->Init();
  //VRB_CHECK(glBindAttribLocation(m.renderState->Program(), 0, "a_position"));
  sPOS = glGetAttribLocation(m.renderState->Program(), "a_position");VRB_CHECK()
  VRLOG("sPOS=%d", sPOS);
  for (Group& group: m.groups) {
    VRB_CHECK(glGenBuffers(1, &group.vertexObjectId));
    VRB_CHECK(glBindBuffer(GL_ARRAY_BUFFER, group.vertexObjectId));
    VRB_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * group.vertexCount, nullptr, GL_STATIC_DRAW));
    VRLOG("Allocate: %d", sizeof(GLfloat) * 3 * group.vertexCount);
    std::vector<GLushort> indicies;
    GLushort count = 0;
    GLintptr offset = 0;
    const GLintptr vertexSize = sizeof(GLfloat) * 3;
    for (auto face: group.faces) {
      const GLushort vertexIndex = face.vertices[0] - 1;
      const Vector& firstVertex = m.vertices[vertexIndex];
      if (face.vertices.size() < 3) {
        std::string message;
        for (auto index: face.vertices) { message += " "; message += std::to_string(index); }
        VRLOG("ERROR! Face with only %d verticies:%s", face.vertices.size(), message.c_str());
        continue;
      }
      for (int ix = 1; ix <= face.vertices.size() - 2; ix++) {
        std::string out;
        VRB_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, vertexSize, firstVertex.Data()));
        indicies.push_back(count);
        out += " " + firstVertex.ToString();
        count++;
        offset += vertexSize;
        VRB_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, vertexSize, m.vertices[face.vertices[ix] - 1].Data()));
        out += "/" + m.vertices[face.vertices[ix] - 1].ToString();
        indicies.push_back(count);
        count++;
        offset += vertexSize;
        VRB_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, vertexSize, m.vertices[face.vertices[ix + 1] - 1].Data()));
        out += "/" + m.vertices[face.vertices[ix + 1] - 1].ToString();
        indicies.push_back(count);
        count++;
        offset += vertexSize;
        VRLOG("array buffer[%d] o:%d c:%d : %s", ix, offset, count, out.c_str());
      }
    }
    VRLOG("indicies: %d vertex: %d", indicies.size(), group.vertexCount);

    VRB_CHECK(glGenBuffers(1, &group.indexObjectId));
    VRB_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, group.indexObjectId));
    VRB_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indicies.size(), indicies.data(), GL_STATIC_DRAW));
  }
}


void
RenderObject::Draw(const Matrix &aProjection) {
//  VRLINE;
  if (m.renderState->Enable(aProjection)) {
    for (Group& group: m.groups) {

      VRB_CHECK(glBindBuffer(GL_ARRAY_BUFFER, group.vertexObjectId));
      VRB_CHECK(glVertexAttribPointer(sPOS, 3, GL_FLOAT, GL_FALSE, 0, nullptr));


      VRB_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, group.indexObjectId));
      VRB_CHECK(glEnableVertexAttribArray(sPOS));
      VRB_CHECK(glDrawElements(GL_TRIANGLES, group.vertexCount, GL_UNSIGNED_SHORT, 0));
      //VRB_CHECK(glDrawArrays(GL_TRIANGLES, 0, group.vertexCount));
      VRB_CHECK(glDisableVertexAttribArray(sPOS));
    }
  }
}

int
RenderObject::AddVertex(const Vector &aVertex) {
  m.vertices.push_back(aVertex);
  return m.vertices.size();
}

int
RenderObject::AddNormal(const Vector &aNormal) {
  m.normals.push_back(aNormal);
  return m.normals.size();
}

int
RenderObject::AddUV(const Vector &aUV) {
  m.uvs.push_back(aUV);
  return m.uvs.size();
}

int
RenderObject::CreateGroup(const std::vector<std::string> &aNames) {
  Group group;
  group.names = aNames;
  m.groups.push_back(group);
  return m.groups.size();
}

int
RenderObject::AddFace(
    const int aGroup,
    const std::vector<int> &aVerticies,
    const std::vector<int> &aUVs,
    const std::vector<int> &aNormals) {
  const int index = aGroup - 1;
  if (index >= m.groups.size()) {
    return -1;
  }

  Group &group = m.groups[index];
  group.faces.push_back(Face());
  const int result = group.faces.size();
  const int faceIndex = result - 1;
  Face &face = group.faces[faceIndex];
  group.vertexCount += aVerticies.size();
  CopyIndecies(face.vertices, aVerticies);
  if (face.vertices.size() < 3) {
    std::string indicies;
    for (auto ix: face.vertices) {
      indicies += " ";
      indicies += std::to_string(ix);
    }
    indicies += " from:";
    for (auto ix: aVerticies) {
      indicies += " ";
      indicies += std::to_string(ix);
    }
    VRLOG("ERROR! Face with only %d verticies:%s", face.vertices.size(), indicies.c_str());
  }
  CopyIndecies(face.uvs, aUVs);
  CopyIndecies(face.normals, aNormals);

  return result;
}

void
RenderObject::Dump() {
  std::cout << "RenderObject: " << m.name << std::endl;
  VRLOG("RenderObject: '%s'", m.name.c_str());
  std::cout << "  Vertices:" << (m.vertices.size() == 0 ? " None" : "") << std::endl;
  VRLOG("  Vertices[%d]:", m.vertices.size());
  for (auto vertex: m.vertices) {
    std::cout << "    " << vertex.ToString() << std::endl;
    VRLOG("    %s", vertex.ToString().c_str());
  }
  std::cout << "  UVs:" << (m.uvs.size() == 0 ? " None" : "") << std::endl;
  VRLOG("  UVs[%d]:", m.uvs.size());
  for (auto uv: m.uvs) {
    std::cout << "    " << uv.ToString() << std::endl;
    VRLOG("    %s", uv.ToString().c_str());
  }
  std::cout << "  Normals:" << (m.normals.size() == 0 ? " None" : "") << std::endl;
  VRLOG("  Normals[%d]:", m.normals.size());
  for (auto normal: m.normals) {
    std::cout << "    " << normal.ToString() << std::endl;
    VRLOG("    %s", normal.ToString().c_str());
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
      std::string faceList;
      const size_t size = face.vertices.size();
      for (size_t ix = 0; ix < size; ix++) {
        faceList += " " + std::to_string(face.vertices[ix]) + "/"
                    + std::to_string(face.uvs[ix])
                    + "/" + std::to_string(face.normals[ix]);
      }
      std::cout << "    f:" << faceList << std::endl;
      VRLOG("    f: %s", faceList.c_str());
    }
  }
}

} // namespace vrb
