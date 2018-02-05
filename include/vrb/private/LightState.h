/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
      , ambient(0.4f, 0.4f, 0.4f)
      , diffuse(1.0f, 1.0f, 1.0f)
      , specular(1.0f, 1.0f, 1.0f)
  {
    direction = direction.Normalize();
  }
};

} // namespace vrb

#endif // VRB_LIGHT_STATE_DOT_H
