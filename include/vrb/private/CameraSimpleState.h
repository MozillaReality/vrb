#ifndef VRB_CAMERA_SIMPLE_STATE_DOT_H
#define VRB_CAMERA_SIMPLE_STATE_DOT_H

#include "vrb/CameraSimple.h"

#include "vrb/Matrix.h"

namespace vrb {

struct CameraSimple::State {
  float width;
  float height;
  float nearClip;
  float farClip;
  float horizontalFOV;
  float verticalFOV;
  Matrix transform;
  Matrix view;
  Matrix perspective;

  State();
  void UpdatePerspective();
};

} // namespace vrb

#endif // VRB_CAMERA_SIMPLE_STATE_DOT_H
