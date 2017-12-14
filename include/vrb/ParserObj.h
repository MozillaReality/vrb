#ifndef VRB_PARSER_OBJ_DOT_H
#define VRB_PARSER_OBJ_DOT_H

#include "vrb/FileReader.h"
#include "vrb/Vector.h"

#include <memory>
#include <string>
#include <vector>

namespace vrb {
class ParserObj;
typedef std::shared_ptr<ParserObj> ParserObjPtr;

class ParserFileReaderObj;
typedef std::shared_ptr<ParserFileReaderObj> ParserFileReaderObjPtr;

class ParserFileReaderObj {
public:
  static ParserFileReaderObjPtr Create(ParserObjPtr aParser, FileReaderPtr aFileReader);
  void ReadRawFile(const std::string& aFileName);

protected:
  ParserFileReaderObj(FileReaderPtr aFileReader, ParserObjPtr aParser);
};

class ParserObserverObj {
public:
  // Geometry Interface
  virtual void StartModel(const std::string& aFile) = 0;
  virtual void FinishModel() = 0;
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

  // Material Interface
  virtual void StartMaterialFile(const std::string& aFileName) = 0;
  virtual void FinishMaterialFile() = 0;
  virtual void CreateMaterial(const std::string& aName ) = 0;
  virtual void SetAmbientColor(const Vector& aColor) = 0;
  virtual void SetDiffuseColor(const Vector& aColor) = 0;
  virtual void SetSpecularColor(const Vector& aColor) = 0;
  virtual void SetSpecularExponent(const float aValue) = 0;
  virtual void SetIlluniationModel(const int aValue) = 0;
  virtual void SetAmbientTexture(const std::string& aFileName) = 0;
  virtual void SetDiffuseTexture(const std::string& aFileName) = 0;
  virtual void SetSpecularTexture(const std::string& aFileName) = 0;

protected:
  ParserObserverObj() {}
  virtual ~ParserObserverObj() {}
};

class ParserObj : public FileHandler {
public:
  static ParserObjPtr Create();

  void SetFileReader(FileReaderPtr aFileReader);
  void ClearFileReader();

  // FileHandler Interface
  void Bind(const std::string& aFileName, const int aFileHandle) override ;
  void LoadFailed(const int aFileHandle, const std::string& aReason) override ;
  void ProcessBuffer(const int aFileHandle, const char* aBuffer, const size_t aSize) override ;
  void Finish(const int aFileHandle) override;

  // ParserObj Interface
  void SetObserver(ParserObserverObj* aObserver);

protected:
  ParserObj();
  ~ParserObj();

  struct State;
  State& m;
};

class ParserObserver_mtl {
public:

};

}
#endif // VRB_PARSER_OBJ_DOT_H
