/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_VERTEX_ARRAY_DOT_H
#define VRB_VERTEX_ARRAY_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class VertexArray {
public:
  static VertexArrayPtr Create(CreationContextPtr& aContext);

  int GetVertexCount() const;
  int GetNormalCount() const;
  int GetUVCount() const;
  int GetColorCount() const;

  void SetNormalCount(const int aCount);

  int GetUVLength() const;
  void SetUVLength(const int aLength);

  const Vector& GetVertex(const int aIndex) const;
  const Vector& GetNormal(const int aIndex) const;
  const Vector& GetUV(const int aIndex) const;
  const Color& GetColor(const int aIndex) const;

  void SetVertex(const int aIndex, const Vector& aPoint);
  void SetNormal(const int aIndex, const Vector& aNormal);
  void SetUV(const int aIndex, const Vector& aUV);
  void SetColor(const int aIndex, const Color& aColor);

  int AppendVertex(const Vector& aPoint);
  int AppendNormal(const Vector& aNormal);
  int AppendUV(const Vector& aUV);
  int AppendColor(const Color& aUV);

  void AddNormal(const int aIndex, const Vector& aNormal);

protected:
  struct State;
  VertexArray(State& aState, CreationContextPtr& aContext);
  ~VertexArray() = default;

private:
  State& m;
  VertexArray() = delete;
  VRB_NO_DEFAULTS(VertexArray);
};

}

#endif //  VRB_VERTEX_ARRAY_DOT_H
