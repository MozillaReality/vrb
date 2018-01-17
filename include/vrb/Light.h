#ifndef VRB_LIGHT_DOT_H
#define VRB_LIGHT_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class Light {
public:
  static LightPtr Create(ContextWeak& aContext);

  const Vector& GetDirection() const;
  const Color& GetAmbientColor() const;
  const Color& GetDiffuseColor() const;
  const Color& GetSpecularColor() const;

  void SetDirection(const Vector& aDirection);
  void SetAmbientColor(const Color& aColor);
  void SetDiffuseColor(const Color& aColor);
  void SetSpecularColor(const Color& aColor);
protected:
  struct State;
  Light(State& aState, ContextWeak& aContext);
  ~Light();
private:
  State& m;
  Light() = delete;
  VRB_NO_DEFAULTS(Light)
};

} // namespace vrb

#endif // VRB_LIGHT_DOT_H
