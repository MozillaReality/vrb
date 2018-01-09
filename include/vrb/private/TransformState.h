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
