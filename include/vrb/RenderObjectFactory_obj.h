#ifndef VRB_RENDER_OBJECT_FACTORY_OBJ_DOT_H
#define VRB_RENDER_OBJECT_FACTORY_OBJ_DOT_H

#include "vrb/Parser_obj.h"
#include "vrb/Mutex.h"
#include "vrb/Vector.h"

#include <vector>

namespace vrb {
// Note: All functions should be called in the same thread except GetLoadedRenderObjects
//       which may be called from any thread.
class RenderObject;

class RenderObjectFactory_obj : public ParserObserver_obj {
public:
  RenderObjectFactory_obj();
  ~RenderObjectFactory_obj();

  void CreateRenderObject(const std::string& aName);
  void FinishRenderObject();
  // Function is thread safe
  void GetLoadedRenderObjects(std::vector<RenderObject*>& aObjects);

  // OBJParserObserver interface
  void LoadMaterialLibrary(const std::string& aFile) override;
  void SetGroupNames(const std::vector<std::string>& aNames) override;
  void SetObjectName(const std::string& aName) override;
  void SetMaterialName(const std::string& aName) override;
  void AddVertex(const Vector& aPoint, const float aW) override;
  void AddNormal(const Vector& aNormal) override;
  void AddUV(const float aU, const float aV, const float aW) override;
  void SetSmoothingGroup(const int aGroup) override;
  void AddFace(
      const std::vector<int>& aVerticies,
      const std::vector<int>& aUVs,
      const std::vector<int>& aNormals) override;

protected:
  Mutex mMutex;
  std::vector<RenderObject*> mLoadedObjects;
  RenderObject* mObject;
  int mGroup;
};

}

#endif // VRB_RENDER_OBJECT_FACTORY_OBJ_DOT_H
