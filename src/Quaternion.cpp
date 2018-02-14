/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/Quaternion.h"
#include "vrb/Matrix.h"

namespace vrb {

void
Quaternion::SetFromRotationMatrix(const Matrix &aMatrix) {
  // Adapted from http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
  const auto & m = *reinterpret_cast<const float (*)[4][4]>(aMatrix.Data());
  const float w = sqrt(fmaxf(0.0f, 1.0f + m[0][0] + m[1][1] + m[2][2])) * 0.5f;
  float x = sqrt(fmaxf(0.0f, 1.0f + m[0][0] - m[1][1] - m[2][2])) * 0.5f;
  float y = sqrt(fmaxf(0.0f, 1.0f - m[0][0] + m[1][1] - m[2][2])) * 0.5f;
  float z = sqrt(fmaxf(0.0f, 1.0f - m[0][0] - m[1][1] + m[2][2])) * 0.5f;

  this->m.mX = copysign(x, m[2][1] - m[1][2]);
  this->m.mY = copysign(y, m[0][2] - m[2][0]);
  this->m.mZ = copysign(z, m[1][0] - m[0][1]);
  this->m.mW = w;
}

}
