#include "vrb/RenderObjectFactoryObj.h"

#include "vrb/Mutex.h"
#include "vrb/RenderObject.h"
#include "vrb/RenderObjectState.h"

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
  vrb::RenderObjectStatePtr state;

  Material () : specularExponent(0.0f) {}
};

}

namespace vrb {

struct RenderObjectFactoryObj::State {
  std::unordered_map<std::string, Material> materials;
  Mutex mutex;
  std::vector<RenderObjectPtr> loadedObjects;
  RenderObjectPtr object;
  int group;
  Material* currentMaterial;

  State()
      : group(0)
      , currentMaterial(nullptr) {}

  void CreateRenderState(Material& aMaterial);
};

void
RenderObjectFactoryObj::State::CreateRenderState(Material& aMaterial) {
  if (aMaterial.state) {
    return;
  }

  aMaterial.state = RenderObjectState::Create();
}

RenderObjectFactoryObj::RenderObjectFactoryObj()
    : m(*(new State)) {

}

RenderObjectFactoryObj::~RenderObjectFactoryObj() {

}

void
RenderObjectFactoryObj::GetLoadedRenderObjects(std::vector<RenderObjectPtr>& aObjects) {
  m.mutex.Lock();
  aObjects = m.loadedObjects;
  m.loadedObjects.clear();
  m.mutex.Unlock();
}

void
RenderObjectFactoryObj::StartModel(const std::string& aFileName) {
  FinishModel();
  m.object = RenderObject::Create(aFileName);
}

void
RenderObjectFactoryObj::FinishModel() {
  if (!m.object) {
    return;
  }
  m.mutex.Lock();
  m.loadedObjects.push_back(m.object);
  m.object.reset();
  m.mutex.Unlock();
}

void
RenderObjectFactoryObj::LoadMaterialLibrary(const std::string& aFile) {

}

void
RenderObjectFactoryObj::SetGroupNames(const std::vector<std::string>& aNames) {
  if (!m.object) { return; }

  m.group = m.object->CreateGroup(aNames);
}

void
RenderObjectFactoryObj::SetObjectName(const std::string& aName) {
  if (!m.object) { return; }
}

void
RenderObjectFactoryObj::SetMaterialName(const std::string& aName) {
  if (!m.object) { return; }
  std::unordered_map<std::string, Material>::iterator it = m.materials.find(aName);
  if (it == m.materials.end()) {
    VRB_LOG("Failed to find material: '%s'", aName.c_str());
    return;
  }

  Material& material = it->second;

  m.CreateRenderState(material);
}

void
RenderObjectFactoryObj::AddVertex(const Vector& aPoint, const float aW) {
  if (!m.object) { return; }
  // VRB_LOG("AddVertex %s", aPoint.ToString().c_str());
  m.object->AddVertex(aPoint);
}

void
RenderObjectFactoryObj::AddNormal(const Vector& aNormal) {
  if (!m.object) { return; }
  // VRB_LOG("AddNormal %s", aNormal.ToString().c_str());
  m.object->AddNormal(aNormal);
}

void
RenderObjectFactoryObj::AddUV(const float aU, const float aV, const float aW) {
  if (!m.object) { return; }
  // VRB_LOG("AddUV %f %f %f", aU, aV, aW);
  m.object->AddUV(Vector(aU, aV, aW));
}

void
RenderObjectFactoryObj::SetSmoothingGroup(const int aGroup) {
  if (!m.object) { return; }
}

void
RenderObjectFactoryObj::AddFace(
    const std::vector<int>& aVerticies,
    const std::vector<int>& aUVs,
    const std::vector<int>& aNormals) {
  if (!m.object) { return; }
  if (!m.group) {
    std::vector<std::string> names;
    names.push_back("");
    m.group = m.object->CreateGroup(names);
  }
  m.object->AddFace(m.group, aVerticies, aUVs, aNormals);
}

void
RenderObjectFactoryObj::StartMaterialFile(const std::string& aFileName) {
  VRB_LOG("StartMaterialFile: '%s'", aFileName.c_str());
}

void
RenderObjectFactoryObj::FinishMaterialFile() {
  VRB_LOG("FinishMaterialFile");
}

void
RenderObjectFactoryObj::CreateMaterial(const std::string& aName ) {
  VRB_LOG("CreateMaterial: '%s'", aName.c_str());
  m.currentMaterial = &m.materials[aName];
}

void
RenderObjectFactoryObj::SetAmbientColor(const Vector& aColor) {
  VRB_LOG("SetAmbientColor: %s", aColor.ToString().c_str());
  if (m.currentMaterial) {
    m.currentMaterial->ambient = aColor;
  }
}

void
RenderObjectFactoryObj::SetDiffuseColor(const Vector& aColor) {
  VRB_LOG("SetDiffuseColor: %s", aColor.ToString().c_str());
  if (m.currentMaterial) {
    m.currentMaterial->diffuse = aColor;
  }
}

void
RenderObjectFactoryObj::SetSpecularColor(const Vector& aColor) {
  VRB_LOG("SetSpecularColor: %s", aColor.ToString().c_str());
  if (m.currentMaterial) {
    m.currentMaterial->specular = aColor;
  }
}

void
RenderObjectFactoryObj::SetSpecularExponent(const float aValue) {
  VRB_LOG("SetSpecularExponent: %f", aValue);
  if (m.currentMaterial) {
    m.currentMaterial->specularExponent = aValue;
  }
}

void
RenderObjectFactoryObj::SetIlluniationModel(const int aValue) {
  VRB_LOG("SetIlluniationModel: %d", aValue);
  if (m.currentMaterial) {

  }
}

void
RenderObjectFactoryObj::SetAmbientTexture(const std::string& aFileName) {
  VRB_LOG("SetAmbientTexture: '%s'", aFileName.c_str());
  if (m.currentMaterial) {
    m.currentMaterial->ambientTextureName = aFileName;
  }
}

void
RenderObjectFactoryObj::SetDiffuseTexture(const std::string& aFileName) {
  VRB_LOG("SetDiffuseTexture: '%s'", aFileName.c_str());
  if (m.currentMaterial) {
    m.currentMaterial->diffuseTextureName = aFileName;
  }
}

void
RenderObjectFactoryObj::SetSpecularTexture(const std::string& aFileName) {
  VRB_LOG("SetSpecularTexture: '%s'", aFileName.c_str());
  if (m.currentMaterial) {
    m.currentMaterial->specularTextureName = aFileName;
  }
}

} // namespace vrb

