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
  m.direction = aDirection;
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
