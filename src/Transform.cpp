/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/Transform.h"
#include "vrb/private/TransformState.h"

#include "vrb/ConcreteClass.h"
#include "vrb/CullVisitor.h"

#include <memory>

namespace vrb {

TransformPtr
Transform::Create(CreationContextPtr& aContext) {
  TransformPtr transform = std::make_shared<ConcreteClass<Transform, Transform::State> > (aContext);
  transform->m.self = transform;
  return transform;
}

void
Transform::Cull(CullVisitor& aVisitor, DrawableList& aDrawables) {
  aVisitor.PushTransform(m.transform);
  Group::Cull(aVisitor, aDrawables);
  aVisitor.PopTransform();
}

const Matrix
Transform::GetWorldTransform() const {
  Matrix result = m.transform;
  std::vector<GroupPtr> parents;
  GetParents(parents);
  while (parents.size() > 0) {
    if (parents.size() > 1) {
      VRB_WARN("Calculating world transform where node has more than one parent");
    }
    GroupPtr parent = parents[0];
    TransformPtr transform = std::dynamic_pointer_cast<Transform>(parent);
    if (transform) {
      result.PreMultiplyInPlace(transform->GetTransform());
    }
    parents.clear();
    parent->GetParents(parents);
  }
  return result;
}

const Matrix&
Transform::GetTransform() const {
  return m.transform;
}

void
Transform::SetTransform(const Matrix& aTransform) {
  m.transform = aTransform;
}

Transform::Transform(State& aState, CreationContextPtr& aContext) : Group(aState, aContext), m(aState) {}
Transform::~Transform() {}

}
