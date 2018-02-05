/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_CAMERA_SIMPLE_DOT_H
#define VRB_CAMERA_SIMPLE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include "vrb/Camera.h"

namespace vrb {

class CameraSimple : public Camera {
public:
  static CameraSimplePtr Create(ContextWeak& aContext);
  // Camera interface
  const Matrix& GetTransform() const override;
  void SetTransform(const Matrix& aTransform) override;
  const Matrix& GetView() const override;
  const Matrix& GetPerspective() const override;

  // CameraSimple interface
  float GetNearClip() const;
  float GetFarClip() const;
  void SetClipRange(const float aNear, const float aFar);
  void SetViewport(const int aWidth, const int aHeight);
  void SetFieldOfView(const float aHorizontal, const float aVertical);
protected:
  struct State;
  CameraSimple(State& aState, ContextWeak& aContext);
  ~CameraSimple();
private:
  State& m;
  CameraSimple() = delete;
  VRB_NO_DEFAULTS(CameraSimple)
};

} // namespace vrb

#endif // VRB_CAMERA_SIMPLE_DOT_H
