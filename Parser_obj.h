#ifndef VRB_PARSER_OBJ_DOT_H
#define VRB_PARSER_OBJ_DOT_H

#include "vrb/Vector.h"

#include <string>
#include <vector>

namespace vrb {

class ParserObserver_obj {
public:
  virtual void LoadMaterialLibrary(const std::string& aFile) = 0;
  virtual void SetGroupNames(const std::vector<std::string>& aNames) = 0;
  virtual void SetObjectName(const std::string& aName) = 0;
  virtual void SetMaterialName(const std::string& aName) = 0;
  virtual void AddVertex(const Vector& aPoint, const float aW) = 0;
  virtual void AddNormal(const Vector& aNormal) = 0;
  virtual void AddUV(const float aU, const float aV, const float aW) = 0;
  virtual void AddFace(
      const std::vector<int>& aVerticies,
      const std::vector<int>& aUVs,
      const std::vector<int>& aNormals) = 0;
  virtual void SetSmoothingGroup(const int aGroup) = 0;
protected:
  ParserObserver_obj() {}
  virtual ~ParserObserver_obj() {}
};

class Parser_obj {
public:
  Parser_obj();
  ~Parser_obj();

  void SetObserver(ParserObserver_obj* aObserver);
  void Start();
  void ParseChunk(const char* aChunk, const size_t aSize);
  void Finish();

protected:
  struct State;
  State& m;
};

class MTLParserObserver {
public:

};

}
#endif // VRB_PARSER_OBJ_DOT_H
