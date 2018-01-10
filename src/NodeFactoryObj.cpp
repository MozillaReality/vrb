#include "vrb/NodeFactoryObj.h"

#include "vrb/ConcreteClass.h"
#include "vrb/Context.h"
#include "vrb/Geometry.h"
#include "vrb/Group.h"
#include "vrb/Mutex.h"
#include "vrb/RenderState.h"
#include "vrb/Vector.h"
#include "vrb/VertexArray.h"

#include <unordered_map>

namespace {

struct Material {
  vrb::Vector ambient;
  vrb::Vector diffuse;
  vrb::Vector specular;
  float specularExponent;
  std::string ambientTextureName;
  std::string diffuseTextureName;
  std::string specularTextureName;
  vrb::RenderStatePtr state;

  Material () : specularExponent(0.0f) {}
};

}

namespace vrb {

struct NodeFactoryObj::State {
  std::unordered_map<std::string, Material> materials;
  std::weak_ptr<Context> context;
  int groupId;
  GroupPtr root;
  VertexArrayPtr vertices;
  GeometryPtr currentGeometry;
  Material* currentMaterial;
  RenderStatePtr defaultRenderState;

  State()
      : groupId(0)
      , currentMaterial(nullptr) {}

  void Reset() {
    groupId = 0;
    vertices = nullptr;
    currentGeometry = nullptr;
    currentMaterial = nullptr;
  }
  void CreateRenderState(Material& aMaterial);
};

void
NodeFactoryObj::State::CreateRenderState(Material& aMaterial) {
  if (aMaterial.state) {
    return;
  }

  aMaterial.state = RenderState::Create(context);
}

NodeFactoryObjPtr
NodeFactoryObj::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<NodeFactoryObj, NodeFactoryObj::State> >(aContext);
}

void
NodeFactoryObj::StartModel(const std::string& aFileName) {
  FinishModel();
  if (!m.root) {
    m.root = Group::Create(m.context);
  }
  m.root->SetName(aFileName);
  m.vertices = VertexArray::Create(m.context);
}

void
NodeFactoryObj::FinishModel() {
  m.Reset();
}

void
NodeFactoryObj::LoadMaterialLibrary(const std::string& aFile) {

}

void
NodeFactoryObj::SetGroupNames(const std::vector<std::string>& aNames) {
  if (!m.root) { return; }
  m.currentGeometry = Geometry::Create(m.context);
  m.root->AddNode(m.currentGeometry);
  m.currentGeometry->SetVertexArray(m.vertices);
  if (!m.defaultRenderState) {
    m.defaultRenderState = RenderState::Create(m.context);
  }
  m.currentGeometry->SetRenderState(m.defaultRenderState);
}

void
NodeFactoryObj::SetObjectName(const std::string& aName) {
}

void
NodeFactoryObj::SetMaterialName(const std::string& aName) {
  std::unordered_map<std::string, Material>::iterator it = m.materials.find(aName);
  if (it == m.materials.end()) {
    VRB_LOG("Failed to find material: '%s'", aName.c_str());
    return;
  }

  Material& material = it->second;

  m.CreateRenderState(material);

  m.currentGeometry->SetRenderState(material.state);
}

void
NodeFactoryObj::AddVertex(const Vector& aPoint, const float aW) {
  // VRB_LOG("AddVertex %s", aPoint.ToString().c_str());
  m.vertices->AddVertex(aPoint);
}

void
NodeFactoryObj::AddNormal(const Vector& aNormal) {
  // VRB_LOG("AddNormal %s", aNormal.ToString().c_str());
  m.vertices->AddNormal(aNormal);
}

void
NodeFactoryObj::AddUV(const float aU, const float aV, const float aW) {
  // VRB_LOG("AddUV %f %f %f", aU, aV, aW);
  m.vertices->AddUV(Vector(aU, aV, aW));
}

void
NodeFactoryObj::SetSmoothingGroup(const int aGroup) {
}

void
NodeFactoryObj::AddFace(
    const std::vector<int>& aVerticies,
    const std::vector<int>& aUVs,
    const std::vector<int>& aNormals) {
  if (!m.currentGeometry) {
    std::vector<std::string> names;
    SetGroupNames(names);
  }
  m.currentGeometry->AddFace(aVerticies, aUVs, aNormals);
}

void
NodeFactoryObj::StartMaterialFile(const std::string& aFileName) {
  VRB_LOG("StartMaterialFile: '%s'", aFileName.c_str());
}

void
NodeFactoryObj::FinishMaterialFile() {
  VRB_LOG("FinishMaterialFile");
}

void
NodeFactoryObj::CreateMaterial(const std::string& aName ) {
  VRB_LOG("CreateMaterial: '%s'", aName.c_str());
  m.currentMaterial = &m.materials[aName];
}

void
NodeFactoryObj::SetAmbientColor(const Vector& aColor) {
  VRB_LOG("SetAmbientColor: %s", aColor.ToString().c_str());
  if (m.currentMaterial) {
    m.currentMaterial->ambient = aColor;
  }
}

void
NodeFactoryObj::SetDiffuseColor(const Vector& aColor) {
  VRB_LOG("SetDiffuseColor: %s", aColor.ToString().c_str());
  if (m.currentMaterial) {
    m.currentMaterial->diffuse = aColor;
  }
}

void
NodeFactoryObj::SetSpecularColor(const Vector& aColor) {
  VRB_LOG("SetSpecularColor: %s", aColor.ToString().c_str());
  if (m.currentMaterial) {
    m.currentMaterial->specular = aColor;
  }
}

void
NodeFactoryObj::SetSpecularExponent(const float aValue) {
  VRB_LOG("SetSpecularExponent: %f", aValue);
  if (m.currentMaterial) {
    m.currentMaterial->specularExponent = aValue;
  }
}

void
NodeFactoryObj::SetIlluniationModel(const int aValue) {
  VRB_LOG("SetIlluniationModel: %d", aValue);
  if (m.currentMaterial) {

  }
}

void
NodeFactoryObj::SetAmbientTexture(const std::string& aFileName) {
  VRB_LOG("SetAmbientTexture: '%s'", aFileName.c_str());
  if (m.currentMaterial) {
    m.currentMaterial->ambientTextureName = aFileName;
  }
}

void
NodeFactoryObj::SetDiffuseTexture(const std::string& aFileName) {
  VRB_LOG("SetDiffuseTexture: '%s'", aFileName.c_str());
  if (m.currentMaterial) {
    m.currentMaterial->diffuseTextureName = aFileName;
  }
}

void
NodeFactoryObj::SetSpecularTexture(const std::string& aFileName) {
  VRB_LOG("SetSpecularTexture: '%s'", aFileName.c_str());
  if (m.currentMaterial) {
    m.currentMaterial->specularTextureName = aFileName;
  }
}

// NodeFactoryObj interface
void
NodeFactoryObj::SetModelRoot(GroupPtr aGroup) {
  m.root = aGroup;
}

GroupPtr&
NodeFactoryObj::GetModelRoot() {
  return m.root;
}

NodeFactoryObj::NodeFactoryObj(State& aState, ContextWeak& aContext) : m(aState) {
  m.context = aContext;
}
NodeFactoryObj::~NodeFactoryObj() {}

} // namespace vrb

