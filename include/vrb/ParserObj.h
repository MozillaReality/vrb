/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_PARSER_OBJ_DOT_H
#define VRB_PARSER_OBJ_DOT_H

#include "vrb/Forward.h"

#include "vrb/FileReader.h"
#include "vrb/MacroUtils.h"

#include <memory>
#include <string>
#include <vector>

namespace vrb {

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
private:
  VRB_NO_DEFAULTS(ParserObserverObj)
};

class ParserObj : public FileHandler {
public:
  static ParserObjPtr Create(ContextWeak& aContext);
  // FileHandler Interface
  void BindFileHandle(const std::string& aFileName, const int aFileHandle) override ;
  void LoadFailed(const int aFileHandle, const std::string& aReason) override ;
  void ProcessRawFileChunk(const int aFileHandle, const char* aBuffer, const size_t aSize) override ;
  void FinishRawFile(const int aFileHandle) override;
  void ProcessImageFile(const int aFileHandle, std::unique_ptr<uint8_t[]>& aImage, const int aWidth, const int aHeight) override;

  // ParserObj Interface
  void LoadModel(const std::string& aFileName);
  void SetFileReader(FileReaderPtr aFileReader);
  void ClearFileReader();
  void SetObserver(ParserObserverObjPtr aObserver);

protected:
  struct State;
  ParserObj(State& aState, ContextWeak& aContext);
  ~ParserObj();
private:
  State& m;
  ParserObj() = delete;
  VRB_NO_DEFAULTS(ParserObj)
};

}
#endif // VRB_PARSER_OBJ_DOT_H
