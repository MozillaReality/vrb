#include "Vector.h"

#include <string>
#include <vector>

namespace vrb {

class OBJParserObserver {
public:
  virtual void LoadMaterialLibrary(const std::string& aFile) = 0;
  virtual void SetGroupNames(const std::vector<std::string>& aNames) = 0;
  virtual void SetObjectName(const std::string& aName) = 0;
  virtual void SetMaterialName(const std::string& aName) = 0;
  virtual void AddVertex(const Vector& aPoint, const float aW) = 0;
  virtual void AddNormal(const Vector& aDirection) = 0;
  virtual void AddUV(const float aU, const float aV, const float aW) = 0;
  virtual void StartFace() = 0;
  virtual void SetSmoothingGroup(const int aGroup) = 0;
  virtual void AddFaceVertex(const int aVertexIndex, const int aNormalIndex, const int aUVIndex) = 0;
protected:
  OBJParserObserver() {}
  virtual ~OBJParserObserver() {}
};

class OBJParser {
public:
  OBJParser();
  ~OBJParser();

  void SetObserver(OBJParserObserver* aObserver);
  void Start();
  void ParseChunk(const char* aChunk, const size_t aSize);
  void Finish();

protected:
  struct State;
  State& m;
};

}
