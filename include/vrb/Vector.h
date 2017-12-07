#ifndef VRB_VECTOR_DOT_H
#define VRB_VECTOR_DOT_H

#include "vrb/Logger.h"

#include <cmath>
#include <string>

namespace vrb {

class Vector {
public:
  Vector() {}
  Vector(const float aX, const float aY, const float aZ) : mData(aX, aY, aZ) {}
  Vector(const Vector& aValue) : mData(aValue.mData) {}
  float& x() { return mData.mX; }
  float& y() { return mData.mY; }
  float& z() { return mData.mZ; }
  float x() const { return mData.mX; }
  float y() const { return mData.mY; }
  float z() const { return mData.mZ; }

  Vector& operator*=(const float aValue) {
    mData.mX *= aValue;
    mData.mY *= aValue;
    mData.mZ *= aValue;
    return *this;
  }

  Vector& operator/=(const float aValue) {
    if (aValue != 0.0f) {
      mData.mX /= aValue;
      mData.mY /= aValue;
      mData.mZ /= aValue;
    }
    return *this;
  }

  float Magnitude() const {
    return std::sqrt(
        (mData.mX * mData.mX) +
        (mData.mY * mData.mY) +
        (mData.mZ * mData.mZ));
  }

  Vector Normalize() const {
    Vector result(*this);
    const float magnitude (Magnitude());
    if (magnitude > 0.0f) { result *= 1.0f / magnitude; }
    return result;
  }

  float* Data() { return &mData.mV[0]; }
  const float* Data() const { return &mData.mV[0]; }

  std::string ToString() const {
    std::string result("(");
    result += std::to_string(mData.mX) + ", "
        + std::to_string(mData.mY) + ", "
        + std::to_string(mData.mZ) + ")";
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

  data_t mData;
};

}

#endif // VRB_VECTOR_DOT_H
