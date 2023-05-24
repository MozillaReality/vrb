/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_QUATERNION_DOT_H
#define VRB_QUATERNION_DOT_H

#include "vrb/Logger.h"

#include <cmath>
#include <string>

namespace vrb {

class Matrix;

class Quaternion {
public:
  Quaternion() {}
  Quaternion(const float aX, const float aY, const float aZ, const float aW) : m(aX, aY, aZ, aW) {}
  Quaternion(const Quaternion& aValue) : m(aValue.m) {}
  // Assumes [x, y, z, w] data layout
  Quaternion(const float aData[4]) : m(aData[0], aData[1], aData[2], aData[3]) {}
  Quaternion(const Matrix &aMatrix) { SetFromRotationMatrix(aMatrix); }

  float& x() { return m.mX; }
  float& y() { return m.mY; }
  float& z() { return m.mZ; }
  float& w() { return m.mW; }
  float x() const { return m.mX; }
  float y() const { return m.mY; }
  float z() const { return m.mZ; }
  float w() const { return m.mW; }

  Quaternion& operator=(const Quaternion& aValue) {
    m = aValue.m;
    return *this;
  }

  Quaternion& operator*=(const float aValue) {
    m.mX *= aValue;
    m.mY *= aValue;
    m.mZ *= aValue;
    m.mW *= aValue;
    return *this;
  }

  Quaternion& operator/=(const float aValue) {
    if (aValue != 0.0f) {
      m.mX /= aValue;
      m.mY /= aValue;
      m.mZ /= aValue;
      m.mW /= aValue;
    }
    return *this;
  }

  Quaternion operator+(const Quaternion &aQuat) const {
      return Quaternion(this->x() + aQuat.x(), this->y() + aQuat.y(), this->z() + aQuat.z(), this->w() + aQuat.w());
  }

  Quaternion& operator*=(const Quaternion &aQuat) {
    *this = *this * aQuat;
    return *this;
  }

  Quaternion operator*(const float aValue) const {
    return Quaternion(*this) *= aValue;
  }

  Quaternion operator/(const float aValue) const {
    return Quaternion(*this) /= aValue;
  }

  Quaternion operator*(const Quaternion &aQuat) const {
    Quaternion q;
    const float bx = aQuat.m.mX, by = aQuat.m.mY, bz = aQuat.m.mZ, bw = aQuat.m.mW;

    q.m.mX = m.mX * bw + m.mW * bx + m.mY * bz - m.mZ * by;
    q.m.mY = m.mY * bw + m.mW * by + m.mZ * bx - m.mX * bz;
    q.m.mZ = m.mZ * bw + m.mW * bz + m.mX * by - m.mY * bx;
    q.m.mW = m.mW * bw - m.mX * bx - m.mY * by - m.mZ * bz;
    return q;
  }

  // Assumes upper 3x3 of aMatrix is a pure rotation matrix (no scaling)
  void SetFromRotationMatrix(const Matrix &aMatrix);

  float Length() const {
    return sqrtf(m.mX * m.mX + m.mY * m.mY + m.mZ * m.mZ + m.mW * m.mW);
  }

  Quaternion Conjugate() const {
    return Quaternion(-m.mX, -m.mY, -m.mZ, m.mW);
  }

  Quaternion Normalize() const {
    float l = Length();
    if (l != 0.0f) {
      l = 1.0f / l;
      return *this * l;
    } else {
      return Quaternion();
    }
  }

  Quaternion Inverse() const {
    return Conjugate().Normalize();
  }

  float DotProduct(const Quaternion& aQuat) {
    return this->x() * aQuat.x() + this->y() * aQuat.y() + this->z() * aQuat.z() + this->w() + aQuat.w();
  }

  // SLERP: Spherical Linear Interpolation
  static Quaternion Slerp(const Quaternion& startQuat, const Quaternion& endQuat, float t);

  void ToEulerAngles(float& aX, float& aY, float& aZ) const;
  void SetFromEulerAngles(float aX, float aY, float aZ);

  float* Data() { return &m.mQ[0]; }
  const float* Data() const { return &m.mQ[0]; }

  std::string ToString() const {
    std::string result("(");
    result += std::to_string(m.mX) + ", "
        + std::to_string(m.mY) + ", "
        + std::to_string(m.mZ) + ", "
        + std::to_string(m.mW) + ")";
    return result;
  }

protected:
  union Data {
    float mQ[4];
    struct {
      float mX, mY, mZ, mW;
    };
    Data() : mX(0.0f), mY(0.0f), mZ(0.0f), mW(1.0f) {}
    Data(const float aX, const float aY, const float aZ, const float aW) : mX(aX), mY(aY), mZ(aZ), mW(aW) {}
    Data(const Data& aData) : mX(aData.mX), mY(aData.mY), mZ(aData.mZ), mW(aData.mW) {}
    Data& operator=(const Data& aData) {
      mX = aData.mX;
      mY = aData.mY;
      mZ = aData.mZ;
      mW = aData.mW;
      return *this;
    }
  };

  union Data m;
};

}

#endif // VRB_QUATERNION_DOT_H
