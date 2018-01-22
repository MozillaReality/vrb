#ifndef VRB_LIGHT_STATE_DOT_H
#define VRB_LIGHT_STATE_DOT_H

#include "vrb/Light.h"

#include "vrb/Color.h"
#include "vrb/Vector.h"

namespace vrb {

struct Light::State {

  Vector direction;
  Color ambient;
  Color diffuse;
  Color specular;

  State()
      : direction(-1.0f, -1.0f, -1.0f)
      , ambient(0.2f, 0.2f, 0.2f)
      , diffuse(1.0f, 1.0f, 1.0f)
      , specular(1.0f, 1.0f, 1.0f)
  {
    direction = direction.Normalize();
  }
};

} // namespace vrb

#endif // VRB_LIGHT_STATE_DOT_H
