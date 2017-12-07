#ifndef VRB_MATRIX_DOT_H
#define VRB_MATRIX_DOT_H

#include "vrb/Vector.h"
#include "vrb/Logger.h"

#include <cmath>

namespace vrb {

const float PI_FLOAT = float(M_PI);

class Matrix {
public:
  static Matrix Identity() {
    return Matrix(
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f);
  }

  static Matrix Position(const Vector& aPosition) {
    return Identity().TranslateInPlace(aPosition);
  }

  static Matrix Translation(const Vector& aTranslation) {
    return Identity().TranslateInPlace(aTranslation);
  }

  static Matrix Rotation(const Vector& aAxis, const float aRotation) {
    Matrix result;
    Vector normalized(aAxis.Normalize());
    const float x = normalized.x();
    const float y = normalized.y();
    const float z = normalized.z();
    const float angleSin = std::sinf(aRotation);
    const float angleCos = std::cosf(aRotation);
    const float oneMinusAngleCos(1.0f - angleCos);
//VRLOG("x=%f y=%f z=%f angleSin=%f angleCos=%f oneMinusAngleCos=%f", x, y, z, angleSin, angleCos, oneMinusAngleCos);
    result.mData.m[0][0] =      (angleCos) + (x * x * oneMinusAngleCos);
    result.mData.m[1][0] = (-z * angleSin) + (x * y * oneMinusAngleCos);
    result.mData.m[2][0] =  (y * angleSin) + (x * z * oneMinusAngleCos);
    result.mData.m[0][1] =  (z * angleSin) + (y * x * oneMinusAngleCos);
    result.mData.m[1][1] =      (angleCos) + (y * y * oneMinusAngleCos);
    result.mData.m[2][1] = (-x * angleSin) + (y * z * oneMinusAngleCos);
    result.mData.m[0][2] = (-y * angleSin) + (z * x * oneMinusAngleCos);
    result.mData.m[1][2] =  (x * angleSin) + (z * y * oneMinusAngleCos);
    result.mData.m[2][2] =      (angleCos) + (z * z * oneMinusAngleCos);
    result.mData.m[3][3] = 1;

    return result;
  }

  static Matrix PerspectiveMatrix(
      const float aLeft, const float aRight, const float aTop, const float aBottom,
      const float aNear, const float aFar) {

    Matrix result;

    const float left = -std::tanf(aLeft) * aNear;
    const float right = std::tanf(aRight) * aNear;
    const float bottom = -std::tanf(aBottom) * aNear;
    const float top = std::tanf(aTop) * aNear;
    VRLOG("left=%f right=%f bottom=%f top=%f",left,right,bottom,top);

    if ((left < right) && (bottom < top) &&
        (aNear < aFar) && (aNear > 0.0f) && (aFar > 0.0f)) {

      const float a00 = (2 * aNear) / (right - left);
      const float a11 = (2 * aNear) / (top - bottom);
      const float a20 = (right + left) / (right - left);
      const float a21 = (top + bottom) / (top - bottom);
      const float a22 = (aNear + aFar) / (aNear - aFar);
      const float a32 = (2 * aNear * aFar) / (aNear - aFar);

      result.mData.m[0][0] = a00;
      result.mData.m[2][0] = a20;
      result.mData.m[1][1] = a11;
      result.mData.m[2][1] = a21;
      result.mData.m[2][2] = a22;
      result.mData.m[3][2] = a32;
      result.mData.m[2][3] = -1;
    }

    return result;
  }

  static Matrix PerspectiveMatrixFromDegrees(
      const float aLeft, const float aRight, const float aTop, const float aBottom,
      const float aNear, const float aFar) {
    return PerspectiveMatrix(
        aLeft * PI_FLOAT / 180.0f, aRight * PI_FLOAT / 180.0f,
        aTop * PI_FLOAT / 180.0f, aBottom * PI_FLOAT / 180.0f,
        aNear, aFar);
  }

  static Matrix PerspectiveMatrixWithResolution(
      const float aWidth, const float aHeight,
      const float aFovX, const float aFovY,
      const float aNear, const float aFar) {
    float fovX = aFovX, fovY = aFovY;
    if ((aFovX <= 0.0f) && (aFovY <= 0.0f)) {
      if (aWidth > aHeight) { fovX = 60.0f; }
      else { fovY = 60.0f; }
    }
    if (fovY <= 0.0f) {
      fovY = 2.0f * std::atanf(std::tanf(fovX * 0.5f) * (aHeight / aWidth));
    } else if (fovX <= 0.0f) {
      fovX = 2.0f * std::atanf(std::tanf(fovY * 0.5f) * (aWidth / aHeight));
    }
    fovX *= 0.5f;
    fovY *= 0.5f;
    VRLOG("fovX=%f fovX=%f fovY=%f fovY=%f aNear=%f aFar=%f", fovX, fovX, fovY, fovY, aNear, aFar);
    return PerspectiveMatrix(fovX, fovX, fovY, fovY, aNear, aFar);
  }

  static Matrix PerspectiveMatrixWithResolutionDegrees(
      const float aWidth, const float aHeight,
      const float aFovX, const float aFovY,
      const float aNear, const float aFar) {
    return PerspectiveMatrixWithResolution(
        aWidth, aHeight,
        aFovX > 0.0f ? aFovX * PI_FLOAT / 180.0f : aFovX,
        aFovY > 0.0f ? aFovY * PI_FLOAT / 180.0f : aFovY,
        aNear, aFar);
  }

  Matrix() : mData(
      0.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 0.0f) {}

  Matrix(
      float a00, float a01, float a02, float a03,
      float a10, float a11, float a12, float a13,
      float a20, float a21, float a22, float a23,
      float a30, float a31, float a32, float a33) :
      mData(
      a00, a01, a02, a03,
      a10, a11, a12, a13,
      a20, a21, a22, a23,
      a30, a31, a32, a33) {}

  Matrix(const Matrix& aValue) : mData(aValue.mData) {}

  Matrix& operator=(const Matrix& aMatrix) {
    mData.Copy(aMatrix.mData);
    return *this;
  }

  Vector MultiplyPosition(const Vector& aPosition) {
    Vector result;
    result.x() = mData.m00 * aPosition.x() + mData.m10 * aPosition.y() + mData.m20 * aPosition.z() + mData.m30;
    result.y() = mData.m01 * aPosition.x() + mData.m11 * aPosition.y() + mData.m21 * aPosition.z() + mData.m31;
    result.z() = mData.m02 * aPosition.x() + mData.m12 * aPosition.y() + mData.m22 * aPosition.z() + mData.m32;
    const float w = mData.m03 * aPosition.x() + mData.m13 * aPosition.y() + mData.m23 * aPosition.z() + mData.m33;
    if ((w != 0) && (w != 1)) {
      result /= w;
    }

    return result;
  }

  Vector MultiplyDirection(const Vector& aPosition) {
    Vector result;
    result.x() = mData.m00 * aPosition.x() + mData.m10 * aPosition.y() + mData.m20 * aPosition.z() + mData.m30;
    result.y() = mData.m01 * aPosition.x() + mData.m11 * aPosition.y() + mData.m21 * aPosition.z() + mData.m31;
    result.z() = mData.m02 * aPosition.x() + mData.m12 * aPosition.y() + mData.m22 * aPosition.z() + mData.m32;
    const float w = mData.m03 * aPosition.x() + mData.m13 * aPosition.y() + mData.m23 * aPosition.z();
    if ((w != 0) && (w != 1)) {
      result /= w;
    }

    return result;
  }

  Matrix PreMultiply(const Matrix& aMatrix) const {
    Matrix result;
    for(int ix = 0; ix < 4; ix++) {
      for(int jy = 0; jy < 4; jy++) {
        float sum = 0;
        for(int kz = 0; kz < 4; kz++) {
           sum += aMatrix.mData.m[kz][jy] * mData.m[ix][kz];
        }
        result.mData.m[ix][jy] = sum;
      }
    }

    return result;
  }

  Matrix PostMultiply(const Matrix& aMatrix) const {
    Matrix result;
    for(int ix = 0; ix < 4; ix++) {
      for(int jy = 0; jy < 4; jy++) {
        float sum = 0;
        for(int kz = 0; kz < 4; kz++) {
           sum += mData.m[kz][jy] * aMatrix.mData.m[ix][kz];
        }
        result.mData.m[ix][jy] = sum;
      }
    }

    return result;
  }

  Matrix& PreMultiplyInPlace(const Matrix& aMatrix) {
    *this = PreMultiply(aMatrix);
    return *this;
  }

  Matrix& PostMultiplyInPlace(const Matrix& aMatrix) {
    *this = PostMultiply(aMatrix);
    return *this;
  }

  Matrix Transpose() const {
    return Matrix(
      mData.m00, mData.m10, mData.m20, mData.m30,
      mData.m01, mData.m11, mData.m21, mData.m31,
      mData.m02, mData.m12, mData.m22, mData.m32,
      mData.m03, mData.m13, mData.m23, mData.m33);
  }

  Matrix& TranslateInPlace(const Vector& aOffset) {
    mData.m[3][0] += aOffset.x();
    mData.m[3][1] += aOffset.y();
    mData.m[3][2] += aOffset.z();
    return *this;
  }

  Matrix& ScaleInPlace(const Vector& aScale) {
    mData.m[0][0] *= aScale.x();
    mData.m[1][1] *= aScale.y();
    mData.m[2][2] *= aScale.z();
    return *this;
  }

  Matrix Inverse() const {
    const float m00 = mData.m00,  m10 = mData.m10,  m20 = mData.m20,  m30 = mData.m30;
    const float m01 = mData.m01,  m11 = mData.m11,  m21 = mData.m21,  m31 = mData.m31;
    const float m02 = mData.m02,  m12 = mData.m12,  m22 = mData.m22,  m32 = mData.m32;

    const float c00 =   m11*m22 - m12*m21,   c10 = -(m01*m22 - m02*m21),  c20 =   m01*m12 - m02*m11;
    const float c01 = -(m10*m22 - m12*m20),  c11 =   m00*m22 - m02*m20,   c21 = -(m00*m12 - m02*m10);
    const float c02 =   m10*m21 - m11*m20,   c12 = -(m00*m21 - m01*m20),  c22 =   m00*m11 - m01*m10;

    const float det = m00*c00 + m10*c10 + m20 * c20;
    if (std::fabsf(det) < 0.00001) {
      return Matrix::Identity();
   }
    const float i00 = c00 / det,  i10 = c01 / det,  i20 = c02 / det;
    const float i01 = c10 / det,  i11 = c11 / det,  i21 = c12 / det;
    const float i02 = c20 / det,  i12 = c21 / det,  i22 = c22 / det;

    return Matrix(
        i00, i01, i02, 0.0f,
        i10, i11, i12, 0.0f,
        i20, i21, i22, 0.0f,
        -(i00*m30 + i10*m31 + i20*m32),
        -(i01*m30 + i11*m31 + i21*m32),
        -(i02*m30 + i12*m31 + i22*m32),
        1);
  }

  float* Data() { return reinterpret_cast<float*>(&(mData.m)); }
  const float* Data () const { return reinterpret_cast<const float*>(&(mData.m)); }

  std::string ToString() const {
    std::string result;
    for(int ix = 0; ix < 4; ix++) {
      result += "[";
      for(int jy = 0; jy < 4; jy++) {
        if (jy != 0) { result += ", "; }
        result += std::to_string(mData.m[ix][jy]);

      }
      result += "]";
    }
    return result;
  }

protected:
  typedef union data {
    float m[4][4];
    struct {
      float m00, m01, m02, m03;
      float m10, m11, m12, m13;
      float m20, m21, m22, m23;
      float m30, m31, m32, m33;
    };

    void Copy(const data& aData) {
      for(int ix = 0; ix < 4; ix++) {
        for(int jy = 0; jy < 4; jy++) {
          m[ix][jy] = aData.m[ix][jy];
        }
      }
    }

    data(
        float a00, float a01, float a02, float a03,
        float a10, float a11, float a12, float a13,
        float a20, float a21, float a22, float a23,
        float a30, float a31, float a32, float a33) :
        m00(a00), m01(a01), m02(a02), m03(a03),
        m10(a10), m11(a11), m12(a12), m13(a13),
        m20(a20), m21(a21), m22(a22), m23(a23),
        m30(a30), m31(a31), m32(a32), m33(a33) {}

    data(const data& aData) {
      Copy(aData);
    }
  } data_t;

  data_t mData;
};

}

#endif // VRB_MATRIX_DOT_H