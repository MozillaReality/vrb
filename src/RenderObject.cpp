#include "vrb/RenderObject.h"

#include "vrb/Base.h"
#include "vrb/GLError.h"
#include "vrb/Logger.h"
#include "vrb/Matrix.h"
#include "vrb/RenderObjectState.h"
#include "vrb/Vector.h"

#include <GLES2/gl2.h>
#include <vector>
#include <iostream>

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
  int triangleCount;
  GLuint vertexObjectId;
  GLuint indexObjectId;

  Group()
      : vertexCount(0), triangleCount (0), vertexObjectId(0), indexObjectId(0) {}
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

struct RenderObject::State {
  std::string name;
  std::vector<Group> groups;
  std::vector<Vector> vertices;
  std::vector<Vector> uvs;
  std::vector<Vector> normals;
  RenderObjectStatePtr renderState;
  Matrix model;

  State() : model(Matrix::Identity()) {}
};

RenderObjectPtr
RenderObject::Create(const std::string &aName) {
  RenderObjectPtr result = std::make_shared<Alloc<RenderObject, RenderObject::State> >();
  result->m->name = aName;
  result->m->renderState = RenderObjectState::Create();
  VRB_LOG("CREATE RenderObject[%p]:'%s'",(void*)result.get(), aName.c_str());
  return result;
}

RenderObject::RenderObject() : m(nullptr) {
}

RenderObject::~RenderObject() {
  VRB_LOG("DESTROY RenderObject[%p]:'%s'", (void *)this, m->name.c_str());
}

std::string
RenderObject::GetName() const {
  return m->name;
}

void
RenderObject::Init() {
  VRB_LINE;
  m->renderState->Init();
  for (Group& group: m->groups) {
    VRB_CHECK(glGenBuffers(1, &group.vertexObjectId));
    VRB_CHECK(glBindBuffer(GL_ARRAY_BUFFER, group.vertexObjectId));
    VRB_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 9 * group.triangleCount, nullptr, GL_STATIC_DRAW));
    VRB_LOG("Allocate: %d", sizeof(GLfloat) * 9 * group.triangleCount);
    std::vector<GLushort> indicies;
    GLushort count = 0;
    GLintptr offset = 0;
    const GLintptr vertexSize = sizeof(GLfloat) * 3;
    for (auto& face: group.faces) {
      if (face.vertices.size() == 0) {
        break;
      }
      const GLushort vertexIndex = face.vertices[0] - 1;
      const Vector& firstVertex = m->vertices[vertexIndex];
      if (face.vertices.size() < 3) {
        std::string message;
        for (auto index: face.vertices) { message += " "; message += std::to_string(index); }
        VRB_LOG("ERROR! Face with only %d verticies:%s", face.vertices.size(), message.c_str());
        continue;
      }
      for (int ix = 1; ix <= face.vertices.size() - 2; ix++) {
        std::string out;
        VRB_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, vertexSize, firstVertex.Data()));
        indicies.push_back(count);
        out += " " + firstVertex.ToString();
        count++;
        offset += vertexSize;
        VRB_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, vertexSize, m->vertices[face.vertices[ix] - 1].Data()));
        out += "/" + m->vertices[face.vertices[ix] - 1].ToString();
        indicies.push_back(count);
        count++;
        offset += vertexSize;
        VRB_CHECK(glBufferSubData(GL_ARRAY_BUFFER, offset, vertexSize, m->vertices[face.vertices[ix + 1] - 1].Data()));
        out += "/" + m->vertices[face.vertices[ix + 1] - 1].ToString();
        indicies.push_back(count);
        count++;
        offset += vertexSize;
        // VRB_LOG("array buffer[%d] o:%d c:%d : %s", ix, (int)offset, count, out.c_str());
      }
    }
    VRB_LOG("indicies: %d vertex: %d", indicies.size(), group.vertexCount);

    VRB_CHECK(glGenBuffers(1, &group.indexObjectId));
    VRB_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, group.indexObjectId));
    VRB_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indicies.size(), indicies.data(), GL_STATIC_DRAW));
    VRB_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    VRB_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
  }
}


void
RenderObject::Draw(const Matrix &aProjection) {
  if (m->renderState->Enable(aProjection.PostMultiply(m->model))) {
    for (Group& group: m->groups) {

      VRB_CHECK(glBindBuffer(GL_ARRAY_BUFFER, group.vertexObjectId));
      VRB_CHECK(glVertexAttribPointer(m->renderState->AttributePosition(), 3, GL_FLOAT, GL_FALSE, 0, nullptr));


      VRB_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, group.indexObjectId));
      VRB_CHECK(glEnableVertexAttribArray(m->renderState->AttributePosition()));
      VRB_CHECK(glDrawElements(GL_TRIANGLES, group.vertexCount, GL_UNSIGNED_SHORT, 0));
      //VRB_CHECK(glDrawArrays(GL_TRIANGLES, 0, group.vertexCount));
      VRB_CHECK(glDisableVertexAttribArray(m->renderState->AttributePosition()));
      VRB_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
      VRB_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }
  }
}

void
RenderObject::SetTransform(const Matrix& aTransform) {
  m->model = aTransform;
}

int
RenderObject::AddVertex(const Vector &aVertex) {
  m->vertices.push_back(aVertex);
  return m->vertices.size();
}

int
RenderObject::AddNormal(const Vector &aNormal) {
  m->normals.push_back(aNormal);
  return m->normals.size();
}

int
RenderObject::AddUV(const Vector &aUV) {
  m->uvs.push_back(aUV);
  return m->uvs.size();
}

int
RenderObject::CreateGroup(const std::vector<std::string> &aNames) {
  Group group;
  group.names = aNames;
  m->groups.push_back(group);
  return m->groups.size();
}

int
RenderObject::AddFace(
    const int aGroup,
    const std::vector<int> &aVerticies,
    const std::vector<int> &aUVs,
    const std::vector<int> &aNormals) {
  const int index = aGroup - 1;
  if (index >= m->groups.size()) {
    return -1;
  }

  Group &group = m->groups[index];
  group.faces.push_back(Face());
  const int result = group.faces.size();
  const int faceIndex = result - 1;
  Face &face = group.faces[faceIndex];
  group.vertexCount += aVerticies.size();
  group.triangleCount += aVerticies.size() - 2;
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
    VRB_LOG("ERROR! Face with only %d verticies:%s", face.vertices.size(), indicies.c_str());
  }
  CopyIndecies(face.uvs, aUVs);
  CopyIndecies(face.normals, aNormals);

  return result;
}

void
RenderObject::Dump() {
  std::cout << "RenderObject: " << m->name << std::endl;
  VRB_LOG("RenderObject: '%s'", m->name.c_str());
  std::cout << "  Vertices:" << (m->vertices.size() == 0 ? " None" : "") << std::endl;
  VRB_LOG("  Vertices[%d]:", m->vertices.size());
  for (auto vertex: m->vertices) {
    std::cout << "    " << vertex.ToString() << std::endl;
    VRB_LOG("    %s", vertex.ToString().c_str());
  }
  std::cout << "  UVs:" << (m->uvs.size() == 0 ? " None" : "") << std::endl;
  VRB_LOG("  UVs[%d]:", m->uvs.size());
  for (auto uv: m->uvs) {
    std::cout << "    " << uv.ToString() << std::endl;
    VRB_LOG("    %s", uv.ToString().c_str());
  }
  std::cout << "  Normals:" << (m->normals.size() == 0 ? " None" : "") << std::endl;
  VRB_LOG("  Normals[%d]:", m->normals.size());
  for (auto normal: m->normals) {
    std::cout << "    " << normal.ToString() << std::endl;
    VRB_LOG("    %s", normal.ToString().c_str());
  }
  for (auto group: m->groups) {
    std::cout << "  Group:";
    std::string nameStr("");
    for (auto name: group.names) {
      std::cout << " '" << name << "'";
      nameStr += " '";
      nameStr += name;
      nameStr += "'";
    }
    std::cout << std::endl;
    VRB_LOG("  Group[%d]: %s", group.faces.size(), nameStr.c_str());
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
      VRB_LOG("    f: %s", faceList.c_str());
    }
  }
}

} // namespace vrb
