#include "vrb/RenderObjectFactoryObj.h"
#include "vrb/RenderObject.h"

namespace vrb {

RenderObjectFactoryObj::RenderObjectFactoryObj()
    : mGroup(0) {

}

RenderObjectFactoryObj::~RenderObjectFactoryObj() {

}

void
RenderObjectFactoryObj::GetLoadedRenderObjects(std::vector<RenderObjectPtr>& aObjects) {
  mMutex.Lock();
  aObjects = mLoadedObjects;
  mLoadedObjects.clear();
  mMutex.Unlock();
}
void
SetFileReader(FileReaderPtr& aFilereader);
void ClearFileReader();
void
RenderObjectFactoryObj::StartModel(const std::string& aFileName) {
  FinishModel();
  mObject = RenderObject::Create(aFileName);
}

void
RenderObjectFactoryObj::FinishModel() {
  if (!mObject) {
    return;
  }
  mMutex.Lock();
  mLoadedObjects.push_back(mObject);
  mObject.reset();
  mMutex.Unlock();
}

void
RenderObjectFactoryObj::LoadMaterialLibrary(const std::string& aFile) {

}

void
RenderObjectFactoryObj::SetGroupNames(const std::vector<std::string>& aNames) {
  if (!mObject) { return; }

  mGroup = mObject->CreateGroup(aNames);
}

void
RenderObjectFactoryObj::SetObjectName(const std::string& aName) {
  if (!mObject) { return; }
}

void
RenderObjectFactoryObj::SetMaterialName(const std::string& aName) {
  if (!mObject) { return; }
}

void
RenderObjectFactoryObj::AddVertex(const Vector& aPoint, const float aW) {
  if (!mObject) { return; }
  // VRLOG("AddVertex %s", aPoint.ToString().c_str());
  mObject->AddVertex(aPoint);
}

void
RenderObjectFactoryObj::AddNormal(const Vector& aNormal) {
  if (!mObject) { return; }
  // VRLOG("AddNormal %s", aNormal.ToString().c_str());
  mObject->AddNormal(aNormal);
}

void
RenderObjectFactoryObj::AddUV(const float aU, const float aV, const float aW) {
  if (!mObject) { return; }
  // VRLOG("AddUV %f %f %f", aU, aV, aW);
  mObject->AddUV(Vector(aU, aV, aW));
}

void
RenderObjectFactoryObj::SetSmoothingGroup(const int aGroup) {
  if (!mObject) { return; }
}

void
RenderObjectFactoryObj::AddFace(
    const std::vector<int>& aVerticies,
    const std::vector<int>& aUVs,
    const std::vector<int>& aNormals) {
  if (!mObject) { return; }
  if (!mGroup) {
    std::vector<std::string> names;
    names.push_back("");
    mGroup = mObject->CreateGroup(names);
  }
  mObject->AddFace(mGroup, aVerticies, aUVs, aNormals);
}

void
RenderObjectFactoryObj::StartMaterialFile(const std::string& aFileName) {

}
void
RenderObjectFactoryObj::FinishMaterialFile() {

}
void
RenderObjectFactoryObj::CreateMaterial(const std::string& aName ) {

}
void
RenderObjectFactoryObj::SetAmbientColor(const Vector& aColor) {

}
void
RenderObjectFactoryObj::SetDiffuseColor(const Vector& aColor) {

}
void
RenderObjectFactoryObj::SetSpecularColor(const Vector& aColor) {

}
void
RenderObjectFactoryObj::SetSpecularExponent(const float aValue) {

}
void
RenderObjectFactoryObj::SetIlluniationModel(const int aValue) {

}
void
RenderObjectFactoryObj::SetAmbientTexture(const std::string& aFileName) {

}
void
RenderObjectFactoryObj::SetDiffuseTexture(const std::string& aFileName) {

}
void
RenderObjectFactoryObj::SetSpecularTexture(const std::string& aFileName) {

}

} // namespace vrb

