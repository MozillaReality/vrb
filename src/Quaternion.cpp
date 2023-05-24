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

void
Quaternion::ToEulerAngles(float& aX, float& aY, float& aZ) const {
  // Adapted from https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
  // roll (x-axis rotation)
  float sinr_cosp = 2.0f * (m.mW * m.mX + m.mY * m.mZ);
  float cosr_cosp = 1.0f - 2.0f * (m.mX * m.mX + m.mY * m.mY);
  aX = atan2(sinr_cosp, cosr_cosp);

  // pitch (y-axis rotation)
  float sinp = 2.0f * (m.mW * m.mY - m.mZ * m.mX);
  if (fabs(sinp) >= 1)
    aY = copysign((float)M_PI / 2, sinp); // use 90 degrees if out of range
  else
    aY = asin(sinp);

  // yaw (z-axis rotation)
  float siny_cosp = 2.0f * (m.mW * m.mZ + m.mX * m.mY);
  float cosy_cosp = 1.0f - 2.0f * (m.mY * m.mY + m.mZ * m.mZ);
  aZ = atan2(siny_cosp, cosy_cosp);
}

void
Quaternion::SetFromEulerAngles(float aX, float aY, float aZ) {
  // Adapted from https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
  float cy = cos(aZ * 0.5f);
  float sy = sin(aZ * 0.5f);
  float cr = cos(aX * 0.5f);
  float sr = sin(aX * 0.5f);
  float cp = cos(aY * 0.5f);
  float sp = sin(aY * 0.5f);

  m.mW = cy * cr * cp + sy * sr * sp;
  m.mX = cy * sr * cp - sy * cr * sp;
  m.mY = cy * cr * sp + sy * sr * cp;
  m.mZ = sy * cr * cp - cy * sr * sp;
}

// Spherical linear interpolation for quaternions based on https://github.com/vrpn/vrpn/blob/master/quat/quat.c
// This methods interpolates from the current quaternion to endQuat. The parameter t goes from 0
// to 1 and so the returned quaternion goes from this to endQuat.
Quaternion
Quaternion::Slerp(const Quaternion& startQuat, const Quaternion& endQuat, float t) {
  const float Q_EPSILON = 1e-10;
  auto startQ = startQuat;
  float startScale;
  float endScale;

  auto cosOmega = startQ.DotProduct(endQuat);
  if (cosOmega < 0.0) {
    cosOmega *= -1;
    startQ *= -1;
  }

  if ((1.0 + cosOmega) > Q_EPSILON) {
    if ((1.0 - cosOmega) > Q_EPSILON ) {
      // usual case
      auto omega = acos(cosOmega);
      auto sinOmega = sin(omega);
      startScale = sin((1.0 - t) * omega) / sinOmega;
      endScale = sin(t * omega) / sinOmega;
    } else {
      // ends very close
      startScale = 1.0 - t;
      endScale = t;
    }
    return startQ * startScale + endQuat * endScale;
  }

  // ends nearly opposite
  auto q = vrb::Quaternion(-startQ.y(), startQ.x(), -startQ.w(), startQ.z());
  startScale = sin((0.5 - t) * M_PI);
  endScale = sin(t * M_PI);
  return startQ * startScale + q * endScale;
};

}
