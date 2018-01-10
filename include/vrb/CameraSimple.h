#ifndef VRB_CAMERA_SIMPLE_DOT_H
#define VRB_CAMERA_SIMPLE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include "vrb/Camera.h"

namespace vrb {

class CameraSimple : public Camera {
public:
  static CameraSimplePtr Create(ContextWeak& aContext);
  // Camera interface
  const Matrix& GetTransform() const override;
  void SetTransform(const Matrix& aTransform) override;
  const Matrix& GetView() const override;
  const Matrix& GetPerspective() const override;

  // CameraSimple interface
  float GetNearClip() const;
  float GetFarClip() const;
  void SetClipRange(const float aNear, const float aFar);
  void SetViewport(const int aWidth, const int aHeight);
  void SetFieldOfView(const float aHorizontal, const float aVertical);
protected:
  struct State;
  CameraSimple(State& aState, ContextWeak& aContext);
  ~CameraSimple();
private:
  State& m;
  CameraSimple() = delete;
  VRB_NO_DEFAULTS(CameraSimple)
};

} // namespace vrb

#endif // VRB_CAMERA_SIMPLE_DOT_H
