/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_CAMERA_EYE_DOT_H
#define VRB_CAMERA_EYE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include "vrb/Camera.h"

namespace vrb {

class CameraEye : public Camera {
public:
  static CameraEyePtr Create(CreationContextPtr& aContext);
  // Camera interface
  const Matrix& GetTransform() const override;
  const Matrix& GetView() const override;
  const Matrix& GetPerspective() const override;

  // CameraEye interface
  void SetPerspective(const Matrix& aPerspective);
  const Matrix& GetHeadTransform() const;
  void SetHeadTransform(const Matrix& aTransform);
  const Matrix& GetEyeTransform() const;
  void SetEyeTransform(const Matrix& aTransform);
protected:
  struct State;
  CameraEye(State& aState, CreationContextPtr& aContext);
  virtual ~CameraEye();
private:
  State& m;
  CameraEye() = delete;
  VRB_NO_DEFAULTS(CameraEye)
};

} // namespace vrb

#endif // VRB_CAMERA_EYE_DOT_H
