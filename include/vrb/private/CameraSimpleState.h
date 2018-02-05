/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_CAMERA_SIMPLE_STATE_DOT_H
#define VRB_CAMERA_SIMPLE_STATE_DOT_H

#include "vrb/CameraSimple.h"

#include "vrb/Matrix.h"

namespace vrb {

struct CameraSimple::State {
  float width;
  float height;
  float nearClip;
  float farClip;
  float horizontalFOV;
  float verticalFOV;
  Matrix transform;
  Matrix view;
  Matrix perspective;

  State();
  void UpdatePerspective();
};

} // namespace vrb

#endif // VRB_CAMERA_SIMPLE_STATE_DOT_H
