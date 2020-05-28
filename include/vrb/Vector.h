/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_VECTOR_DOT_H
#define VRB_VECTOR_DOT_H

#include "vrb/Logger.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>

namespace vrb {

class Vector {
public:
  static const Vector& Zero();
  static const Vector& Min();
  static const Vector& Max();
  Vector() {}
  Vector(const float aX, const float aY, const float aZ) : m(aX, aY, aZ) {}
  Vector(const Vector& aValue) : m(aValue.m) {}
  float& x() { return m.mX; }
  float& y() { return m.mY; }
  float& z() { return m.mZ; }
  float x() const { return m.mX; }
  float y() const { return m.mY; }
  float z() const { return m.mZ; }
  Vector& Set(const float aX, const float aY, const float aZ) {
    m.mX = aX;
    m.mY = aY;
    m.mZ = aZ;
    return *this;
  }

  Vector operator-() const {
    Vector result(-m.mX, -m.mY, -m.mZ);
    return result;
  }

  Vector& operator=(const Vector& aValue) {
    m = aValue.m;
    return *this;
  }

  Vector& operator+=(const Vector& aValue) {
    m.mX += aValue.m.mX;
    m.mY += aValue.m.mY;
    m.mZ += aValue.m.mZ;
    return *this;
  }

  Vector& operator-=(const Vector& aValue) {
    m.mX -= aValue.m.mX;
    m.mY -= aValue.m.mY;
    m.mZ -= aValue.m.mZ;
    return *this;
  }

  Vector& operator*=(const float aValue) {
    m.mX *= aValue;
    m.mY *= aValue;
    m.mZ *= aValue;
    return *this;
  }

  Vector& operator/=(const float aValue) {
    if (aValue != 0.0f) {
      m.mX /= aValue;
      m.mY /= aValue;
      m.mZ /= aValue;
    }
    return *this;
  }

  Vector operator+(const Vector& aValue) const {
    return Vector(
        m.mX + aValue.m.mX,
        m.mY + aValue.m.mY,
        m.mZ + aValue.m.mZ);
  }

  Vector operator-(const Vector& aValue) const {
    return Vector(
        m.mX - aValue.m.mX,
        m.mY - aValue.m.mY,
        m.mZ - aValue.m.mZ);
  }

  Vector operator*(const float aValue) const {
    return Vector(*this) *= aValue;
  }

  Vector operator/(const float aValue) const {
    return Vector(*this) /= aValue;
  }

  bool operator==(const Vector& aVector) const {
    return m.mX == aVector.m.mX && m.mY == aVector.m.mY && aVector.m.mZ == aVector.m.mZ;
  }

  float Magnitude() const {
    return std::sqrt(
        (m.mX * m.mX) +
        (m.mY * m.mY) +
        (m.mZ * m.mZ));
  }

  Vector Normalize() const {
    Vector result(*this);
    const float magnitude = Magnitude();
    if (magnitude > 0.0f) { result *= 1.0f / magnitude; }
    return result;
  }

  float Dot(const Vector &aValue) const {
    return (m.mX * aValue.m.mX) + (m.mY * aValue.m.mY) + (m.mZ * aValue.m.mZ);
  }


  Vector Cross(const Vector &aValue) const {
    return Vector(
        (m.mY * aValue.m.mZ) - (m.mZ * aValue.m.mY),
        (m.mZ * aValue.m.mX) - (m.mX * aValue.m.mZ),
        (m.mX * aValue.m.mY) - (m.mY * aValue.m.mX));
  }

  Vector& ContractInPlace(const Vector& aPoint) {
    m.mX = std::min(m.mX, aPoint.x());
    m.mY = std::min(m.mY, aPoint.y());
    m.mZ = std::min(m.mZ, aPoint.z());
    return *this;
  }

  Vector& ExpandInPlace(const Vector& aPoint) {
    m.mX = std::max(m.mX, aPoint.x());
    m.mY = std::max(m.mY, aPoint.y());
    m.mZ = std::max(m.mZ, aPoint.z());
    return *this;
  }

  Vector Contract(const Vector& aPoint) {
    Vector result(*this);
    return result.ContractInPlace(aPoint);
  }

  Vector Expand(const Vector& aPoint) {
    Vector result(*this);
    return result.ExpandInPlace(aPoint);
  }

  float* Data() { return &m.mV[0]; }
  const float* Data() const { return &m.mV[0]; }

  std::string ToString() const {
    std::string result("(");
    result += std::to_string(m.mX) + ", "
        + std::to_string(m.mY) + ", "
        + std::to_string(m.mZ) + ")";
    return result;
  }

protected:
  union Data {
    float mV[3];
    struct {
      float mX, mY, mZ;
    };
    Data() : mX(0.0f), mY(0.0f), mZ(0.0f) {}
    Data(const float aX, const float aY, const float aZ) : mX(aX), mY(aY), mZ(aZ) {}
    Data(const Data& aData) : mX(aData.mX), mY(aData.mY), mZ(aData.mZ) {}
    Data& operator=(const Data& aData) {
      mX = aData.mX;
      mY = aData.mY;
      mZ = aData.mZ;
      return *this;
    }
  };

  union Data m;
};

}

#endif // VRB_VECTOR_DOT_H
