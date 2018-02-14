/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/CameraEye.h"
#include "vrb/private/CameraEyeState.h"

#include "vrb/ConcreteClass.h"

namespace vrb {

CameraEye::State::State()
    : dirty(false)
    , headTransform(Matrix::Identity())
    , eyeTransform(Matrix::Identity())
    , perspective(Matrix::Identity())
    , transform(Matrix::Identity())
    , view(transform.Inverse())
{}

void
CameraEye::State::Update() {
  dirty = false;
  transform = headTransform.PostMultiply(eyeTransform);
  view = transform.Inverse();
}

CameraEyePtr
CameraEye::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<CameraEye, CameraEye::State> >(aContext);
}

// Camera interface
const Matrix&
CameraEye::GetTransform() const {
  if (m.dirty) { m.Update(); }
  return m.transform;
}

const Matrix&
CameraEye::GetView() const {
  if (m.dirty) { m.Update(); }
  return m.view;
}

const Matrix&
CameraEye::GetPerspective() const {
  return m.perspective;
}

  // CameraEye interface
void
CameraEye::SetPerspective(const Matrix& aPerspective) {
  m.perspective = aPerspective;
}

const Matrix&
CameraEye::GetHeadTransform() const {
  return m.headTransform;
}

void
CameraEye::SetHeadTransform(const Matrix& aTransform) {
  m.headTransform = aTransform;
  m.dirty = true;
}

const Matrix&
CameraEye::GetEyeTransform() const {
  return m.eyeTransform;
}

void
CameraEye::SetEyeTransform(const Matrix& aTransform) {
  m.eyeTransform = aTransform;
  m.dirty = true;
}

CameraEye::CameraEye(State& aState, ContextWeak& aContext) : m(aState) {}
CameraEye::~CameraEye() {}


} // namespace vrb
