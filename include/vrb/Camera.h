#ifndef VRB_CAMERA_DOT_H
#define VRB_CAMERA_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class Camera {
public:
  virtual const Matrix& GetTransform() const = 0;
  virtual void SetTransform(const Matrix& aTransform) = 0;
  virtual const Matrix& GetView() const = 0;
  virtual const Matrix& GetPerspective() const = 0;

protected:
  Camera() {}
  ~Camera() {}
private:
  VRB_NO_DEFAULTS(Camera)
};

} // namespace vrb

#endif // VRB_CAMERA_DOT_H
