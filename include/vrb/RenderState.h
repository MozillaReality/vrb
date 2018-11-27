/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_RENDER_STATE_DOT_H
#define VRB_RENDER_STATE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/ResourceGL.h"

#include "vrb/gl.h"

namespace vrb{

class RenderState : protected ResourceGL {
public:
  static RenderStatePtr Create(CreationContextPtr& aContext);
  GLuint Program() const;
  GLint AttributePosition() const;
  GLint AttributeNormal() const;
  GLint AttributeUV() const;
  uint32_t GetLightId() const;
  void ResetLights(const uint32_t aId);
  void AddLight(const Vector& aDirection, const Color& aAmbient, const Color& aDiffuse, const Color& aSpecular);
  void SetMaterial(const Color& aAmbient, const Color& aDiffuse, const Color& aSpecular, const float aSpecularExponent);
  void SetAmbient(const Color& aColor);
  void SetDiffuse(const Color& aColor);
  void GetMaterial(Color& aAmbient, Color& aDiffuse, Color& aSpecular, float& aSpecularExponent) const;
  TexturePtr GetTexture() const;
  void SetTexture(const TexturePtr& aTexture);
  bool HasTexture() const;
  const Color& GetTintColor() const;
  void SetTintColor(const Color& aColor);
  bool Enable(const Matrix& aPerspective, const Matrix& aView, const Matrix& aModel);
  void Disable();
  void SetLightsEnabled(bool aEnabled);
  void SetFragmentPrecision(const GLenum aPrecision);
protected:
  struct State;
  RenderState(State& aState, CreationContextPtr& aContext);
  ~RenderState();

  // ResourceGL interface
  void InitializeGL() override;
  void ShutdownGL() override;

private:
  State& m;
  RenderState() = delete;
  VRB_NO_DEFAULTS(RenderState)
};

} // namespace vrb

#endif //VRB_RENDER_STATE_DOT_H
