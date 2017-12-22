#ifndef VRB_RENDER_OBJECT_FACTORY_OBJ_DOT_H
#define VRB_RENDER_OBJECT_FACTORY_OBJ_DOT_H

#include "vrb/ParserObj.h"
#include "vrb/RenderObject.h"
#include "vrb/Vector.h"

#include <vector>

namespace vrb {
// Note: All functions should be called in the same thread except GetLoadedRenderObjects
//       which may be called from any thread.
class RenderObject;

class RenderObjectFactoryObj : public ParserObserverObj {
public:
  RenderObjectFactoryObj();
  ~RenderObjectFactoryObj();

  // Function is thread safe
  void GetLoadedRenderObjects(std::vector<RenderObjectPtr>& aObjects);

  // ParserObserverObj interface
  void StartModel(const std::string& aFileName) override;
  void FinishModel() override;
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

  void StartMaterialFile(const std::string& aFileName) override;
  void FinishMaterialFile() override;
  void CreateMaterial(const std::string& aName ) override;
  void SetAmbientColor(const Vector& aColor) override;
  void SetDiffuseColor(const Vector& aColor) override;
  void SetSpecularColor(const Vector& aColor) override;
  void SetSpecularExponent(const float aValue) override;
  void SetIlluniationModel(const int aValue) override;
  void SetAmbientTexture(const std::string& aFileName) override;
  void SetDiffuseTexture(const std::string& aFileName) override;
  void SetSpecularTexture(const std::string& aFileName) override;

protected:
  struct State;
  State &m;
};

}

#endif // VRB_RENDER_OBJECT_FACTORY_OBJ_DOT_H
