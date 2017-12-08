#ifndef VRB_VECTOR_DOT_H
#define VRB_VECTOR_DOT_H

#include "vrb/Logger.h"

#include <cmath>
#include <string>

namespace vrb {

class Vector {
public:
  Vector() {}
  Vector(const float aX, const float aY, const float aZ) : m(aX, aY, aZ) {}
  Vector(const Vector& aValue) : m(aValue.m) {}
  float& x() { return m.mX; }
  float& y() { return m.mY; }
  float& z() { return m.mZ; }
  float x() const { return m.mX; }
  float y() const { return m.mY; }
  float z() const { return m.mZ; }

  Vector operator+(const Vector& aValue) {
    return Vector(
        m.mX + aValue.m.mX,
        m.mY + aValue.m.mY,
        m.mZ + aValue.m.mZ);
  }

  Vector operator-(const Vector& aValue) {
    return Vector(
        m.mX - aValue.m.mX,
        m.mY - aValue.m.mY,
        m.mZ - aValue.m.mZ);
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

  float Magnitude() const {
    return std::sqrt(
        (m.mX * m.mX) +
        (m.mY * m.mY) +
        (m.mZ * m.mZ));
  }

  Vector Normalize() const {
    Vector result(*this);
    const float magnitude (Magnitude());
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
  typedef union data {
    float mV[3];
    struct {
      float mX, mY, mZ;
    };
    data() : mX(0.0f), mY(0.0f), mZ(0.0f) {}
    data(const float aX, const float aY, const float aZ) : mX(aX), mY(aY), mZ(aZ) {}
    data(const data& aData) : mX(aData.mX), mY(aData.mY), mZ(aData.mZ) {}
  } data_t;

  data_t m;
};

}

#endif // VRB_VECTOR_DOT_H
