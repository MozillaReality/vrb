/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/Light.h"
#include "vrb/private/LightState.h"

#include "vrb/ConcreteClass.h"
#include "vrb/Vector.h"

namespace vrb {

LightPtr
Light::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<Light, Light::State> >(aContext);
}

const Vector&
Light::GetDirection() const {
  return m.direction;
}

const Color&
Light::GetAmbientColor() const {
  return m.ambient;
}

const Color&
Light::GetDiffuseColor() const {
  return m.diffuse;
}

const Color&
Light::GetSpecularColor() const {
  return m.specular;
}

void
Light::SetDirection(const Vector& aDirection) {
  if (aDirection.Magnitude() > 0.0f) {
    m.direction = aDirection.Normalize();
  }
}

void
Light::SetAmbientColor(const Color& aColor) {
  m.ambient = aColor;
}

void
Light::SetDiffuseColor(const Color& aColor) {
  m.diffuse = aColor;
}

void
Light::SetSpecularColor(const Color& aColor) {
  m.specular = aColor;
}

Light::Light(State& aState, ContextWeak& aContext) : m(aState) {}
Light::~Light() {}

} // namespace vrb
