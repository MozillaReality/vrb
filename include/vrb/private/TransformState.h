/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_TRANSFORM_STATE_DOT_H
#define VRB_TRANSFORM_STATE_DOT_H

#include "vrb/Matrix.h"
#include "vrb/private/GroupState.h"
#include "vrb/Transform.h"

namespace vrb {

struct Transform::State : public Group::State {
  Matrix transform;
};

}

#endif //  VRB_TRANSFORM_STATE_DOT_H
