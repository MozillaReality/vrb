/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_TEXTURE_CUBE_MAP_DOT_H
#define VRB_TEXTURE_CUBE_MAP_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/ResourceGL.h"
#include "vrb/Texture.h"

#include "vrb/gl.h"
#include <string>

namespace vrb {

class TextureCubeMap : public Texture, protected ResourceGL {
public:
  static TextureCubeMapPtr Create(CreationContextPtr& aContext, GLuint aExternalTexture = 0);

  static void Load(CreationContextPtr& aContext, const TextureCubeMapPtr& aTexture,
                   const std::string& aFileXPos, const std::string& aFileXNeg,
                   const std::string& aFileYPos, const std::string& aFileYNeg,
                   const std::string& aFileZPos, const std::string& aFileZNeg);
  void SetImageData(const GLenum aFaceTarget, std::unique_ptr<uint8_t[]>& aImage, const uint64_t aImageLength, const int aWidth, const int aHeight, const GLenum aFormat);
protected:
  struct State;
  TextureCubeMap(State& aState, CreationContextPtr& aContext);
  ~TextureCubeMap();

  // Texture interface
  void AboutToBind() override;

  // ResourceGL interface
  bool SupportOffRenderThreadInitialization() override;
  void InitializeGL() override;
  void ShutdownGL() override;

private:
  State& m;
  TextureCubeMap() = delete;
  VRB_NO_DEFAULTS(TextureCubeMap)
};

} // namespace vrb

#endif // VRB_TEXTURE_CUBE_MAP_DOT_H
