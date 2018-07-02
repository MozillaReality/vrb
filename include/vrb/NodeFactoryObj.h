/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_NODE_FACTORY_OBJ_DOT_H
#define VRB_NODE_FACTORY_OBJ_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/ParserObj.h"

#include <string>

namespace vrb {

class NodeFactoryObj : public ParserObserverObj {
public:
  static NodeFactoryObjPtr Create(CreationContextPtr& aContext);

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

  // NodeFactoryObj interface
  void SetModelRoot(GroupPtr aGroup);
  GroupPtr& GetModelRoot();

protected:
  struct State;
  NodeFactoryObj(State& aState, CreationContextPtr& aContext);
  ~NodeFactoryObj();

private:
  State &m;
  NodeFactoryObj() = delete;
  VRB_NO_DEFAULTS(NodeFactoryObj);
};

}

#endif // VRB_NODE_FACTORY_OBJ_DOT_H
