/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_COLOR_DOT_H
#define VRB_COLOR_DOT_H

#include "vrb/Vector.h"
#include "string"

namespace vrb {

class Color {
public:
  static const Color& Zero();
  Color() {}
  Color(const Color& aColor) : m(aColor.m) {}
  Color(const float aRed, const float aGreen, const float aBlue, const float aAlpha) : m(aRed, aGreen, aBlue, aAlpha) {}
  Color(const float aRed, const float aGreen, const float aBlue) : m(aRed, aGreen, aBlue, 1.0f) {}
  Color(const int32_t aHexColor) : m(((aHexColor & 0xFF000000) >> 24) / 255.0f,
                                     ((aHexColor & 0xFF0000) >> 16) / 255.0f,
                                     ((aHexColor & 0xFF00) >> 8) / 255.0f,
                                     (aHexColor & 0xFF) / 255.0f) {}

  bool HasAlpha() const { return m.alpha < 1.0f; }
  bool IsBlack() const { return (m.red == 0.0f) && (m.green == 0.0f) && (m.blue == 0.0f); }
  bool IsWhite() const { return (m.red == 1.0f) && (m.green == 1.0f) && (m.blue == 1.0f); }

  Color& operator=(const Color& aColor) {
    for (int ix = 0; ix < 4; ix++) {
      m.v[ix] = aColor.m.v[ix];
    }
    return *this;
  }

  Color& operator=(const Vector& aColor) {
    m.red = aColor.x();
    m.green = aColor.y();
    m.blue = aColor.z();
    m.alpha = 1.0f;
    m.Clip();
    return *this;
  }

  Color& operator+=(const Color& aColor) {
    m.red += aColor.m.red;
    m.green += aColor.m.green;
    m.blue += aColor.m.blue;
    m.alpha += aColor.m.alpha;
    m.Clip();
    return *this;
  }

  Color& operator*=(const Color& aColor) {
    m.red *= aColor.m.red;
    m.green *= aColor.m.green;
    m.blue *= aColor.m.blue;
    m.alpha *= aColor.m.alpha;
    return *this;
  }

  Color operator+(const Color& aColor) const {
    Color result (*this);
    result += aColor;
    return result;
  }

  Color operator*(const Color& aColor) const {
    Color result (*this);
    result *= aColor;
    return result;
  }

  bool operator==(const Color& aColor) const {
    return m.red == aColor.m.red && m.green == aColor.m.green && m.blue == aColor.m.blue && m.alpha == aColor.m.alpha;
  }

  float Red() const { return m.red; }
  float Green() const { return m.green; }
  float Blue() const { return m.blue; }
  float Alpha() const { return m.alpha; }
  void SetRGB(const float aRed, const float aGreen, const float aBlue) {
    m.red = aRed;
    m.green = aGreen;
    m.blue = aBlue;
    m.alpha = 1.0f;
    m.Clip();
  }

  void SetRGBA(const float aRed, const float aGreen, const float aBlue, const float aAlpha) {
    m.red = aRed;
    m.green = aGreen;
    m.blue = aBlue;
    m.alpha = aAlpha;
    m.Clip();
  }

  void SetAlpha(const float aAlpha) {
    m.alpha = aAlpha;
    if (m.alpha > 1.0f) {
      m.alpha = 1.0f;
    } else if (m.alpha < 0.0f) {
      m.alpha = 0.0f;
    }
  }

  std::string ToString() {
    std::string result("(");
    result += std::to_string(m.red) + ", "
        + std::to_string(m.green) + ", "
        + std::to_string(m.blue) + ", "
        + std::to_string(m.alpha) + ")";
    return result;
  }
  float* Data() { return &m.v[0]; }
  const float* Data() const { return &m.v[0]; }
private:
  union Data {
    float v[4];
    struct {
      float red, green, blue, alpha;
    };
    Data() : red(0.0f), green(0.0f), blue(0.0f), alpha(1.0f) {}
    Data(const float aRed, const float aGreen, const float aBlue, const float aAlpha) : red(aRed), green(aGreen), blue(aBlue), alpha(aAlpha) { Clip(); }
    Data(const Data& aData) : red(aData.red), green(aData.green), blue(aData.blue), alpha(aData.alpha) { Clip(); }
    void Clip() {
      for (int ix = 0; ix < 4; ix++) {
        if (v[ix] > 1.0f) { v[ix] = 1.0f; }
        else if (v[ix] < 0.0f) { v[ix] = 0.0f; }
      }
    }
  };

  union Data m;
};

} // namespace vrb

#endif //  VRB_COLOR_DOT_H
