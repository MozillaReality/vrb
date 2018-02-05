/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/CameraSimple.h"
#include "vrb/private//CameraSimpleState.h"

#include "vrb/ConcreteClass.h"

namespace vrb {

CameraSimple::State::State()
    : width(1920.0f)
    , height(1080.0f)
    , nearClip(0.1f)
    , farClip(100.0f)
    , horizontalFOV(60.f)
    , verticalFOV(-1.0f)
    , transform(Matrix::Identity())
    , view(transform.Inverse())
    , perspective(Matrix::PerspectiveMatrixWithResolutionDegrees(
        width, height,
        horizontalFOV, verticalFOV,
        nearClip, farClip))
{}

void
CameraSimple::State::UpdatePerspective() {
  perspective = Matrix::PerspectiveMatrixWithResolutionDegrees(
      width, height,
      horizontalFOV, verticalFOV,
      nearClip, farClip);
}

CameraSimplePtr
CameraSimple::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<CameraSimple, CameraSimple::State> >(aContext);
}

// Camera interface
const Matrix&
CameraSimple::GetTransform() const {
  return m.transform;
}

void
CameraSimple::SetTransform(const Matrix& aTransform) {
  m.transform = aTransform;
  m.view = m.transform.Inverse();
}

const Matrix&
CameraSimple::GetView() const {
  return m.view;
}

const Matrix&
CameraSimple::GetPerspective() const {
  return m.perspective;
}

// CameraSimple interface
float
CameraSimple::GetNearClip() const {
  return m.nearClip;
}

float
CameraSimple::GetFarClip() const {
  return m.farClip;
}

void
CameraSimple::SetClipRange(const float aNear, const float aFar) {
  m.nearClip = aNear;
  m.farClip = aFar;
  m.UpdatePerspective();
}

void
CameraSimple::SetViewport(const int aWidth, const int aHeight) {
  m.width = aWidth;
  m.height = aHeight;
  m.UpdatePerspective();
}

void
CameraSimple::SetFieldOfView(const float aHorizontal, const float aVertical) {
  m.horizontalFOV = aHorizontal;
  m.verticalFOV = aVertical;
  m.UpdatePerspective();
}

CameraSimple::CameraSimple(State& aState, ContextWeak& aContext) : m(aState) {}
CameraSimple::~CameraSimple() {}


} // namespace vrb
