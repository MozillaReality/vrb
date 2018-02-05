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
Transform::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<Transform, Transform::State> > (aContext);
}

void
Transform::Cull(CullVisitor& aVisitor, DrawableList& aDrawables) {
  aVisitor.PushTransform(m.transform);
  Group::Cull(aVisitor, aDrawables);
  aVisitor.PopTransform();
}

const Matrix&
Transform::GetTransform() const {
  return m.transform;
}

void
Transform::SetTransform(const Matrix& aTransform) {
  m.transform = aTransform;
}

Transform::Transform(State& aState, ContextWeak& aContext) : Group(aState, aContext), m(aState) {}
Transform::~Transform() {}

}
