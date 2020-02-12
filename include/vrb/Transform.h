/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_TRANSFORM_DOT_H
#define VRB_TRANSFORM_DOT_H

#include "vrb/Forward.h"
#include "vrb/Group.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class Transform : public Group {
public:
  static TransformPtr Create(CreationContextPtr& aContext);
  // Node interface
  void Cull(CullVisitor& aVisitor, DrawableList& aDrawables) override;
  // Transform interface
  const Matrix GetWorldTransform() const;
  const Matrix& GetTransform() const;
  virtual void SetTransform(const Matrix& aTransform);
protected:
  struct State;
  Transform(State& aState, CreationContextPtr& aContext);
  ~Transform();
private:
  State& m;
  Transform() = delete;
  VRB_NO_DEFAULTS(Transform)
};

}

#endif //  VRB_TRANSFORM_DOT_H
