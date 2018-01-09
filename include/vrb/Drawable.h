#ifndef VRB_DRAWABLE_DOT_H
#define VRB_DRAWABLE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/Node.h"

namespace vrb {

class Drawable : public std::enable_shared_from_this<Drawable> {
public:
  DrawablePtr CreateDrawablePtr();
  virtual void Draw(const CameraPtr& aCamera, const Matrix& aModelTransform) = 0;
protected:
  struct State;
  Drawable(State& aState, ContextWeak& aContext);
  ~Drawable();
private:
  State& m;
  Drawable() = delete;
  VRB_NO_DEFAULTS(Drawable)
};

}

#endif // VRB_DRAWABLE_DOT_H
