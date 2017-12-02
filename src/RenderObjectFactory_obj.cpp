#include "vrb/RenderObjectFactory_obj.h"
#include "vrb/RenderObject.h"

namespace vrb {

RenderObjectFactory_obj::RenderObjectFactory_obj()
    : mGroup(0) {

}

RenderObjectFactory_obj::~RenderObjectFactory_obj() {

}

void
RenderObjectFactory_obj::CreateRenderObject(const std::string& aName) {
  FinishRenderObject();
  mObject = RenderObject::Create(aName);
}

void
RenderObjectFactory_obj::FinishRenderObject() {
  if (!mObject) {
    return;
  }
  mMutex.Lock();
  mLoadedObjects.push_back(mObject);
  mObject.reset();
  mMutex.Unlock();
}

void
RenderObjectFactory_obj::GetLoadedRenderObjects(std::vector<RenderObjectPtr>& aObjects) {
  mMutex.Lock();
  aObjects = mLoadedObjects;
  mLoadedObjects.clear();
  mMutex.Unlock();
}

void
RenderObjectFactory_obj::LoadMaterialLibrary(const std::string& aFile) {

}

void
RenderObjectFactory_obj::SetGroupNames(const std::vector<std::string>& aNames) {
  if (!mObject) { return; }

  mGroup = mObject->CreateGroup(aNames);
}

void
RenderObjectFactory_obj::SetObjectName(const std::string& aName) {
  if (!mObject) { return; }
}

void
RenderObjectFactory_obj::SetMaterialName(const std::string& aName) {
  if (!mObject) { return; }
}

void
RenderObjectFactory_obj::AddVertex(const Vector& aPoint, const float aW) {
  if (!mObject) { return; }

  mObject->AddVertex(aPoint);
}

void
RenderObjectFactory_obj::AddNormal(const Vector& aNormal) {
  if (!mObject) { return; }

  mObject->AddNormal(aNormal);
}

void
RenderObjectFactory_obj::AddUV(const float aU, const float aV, const float aW) {
  if (!mObject) { return; }
  mObject->AddUV(Vector(aU, aV, aW));
}

void
RenderObjectFactory_obj::SetSmoothingGroup(const int aGroup) {
  if (!mObject) { return; }
}

void
RenderObjectFactory_obj::AddFace(
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

} // namespace vrb

