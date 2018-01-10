#ifndef VRB_DRAWABLE_LIST_DOT_H
#define VRB_DRAWABLE_LIST_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class DrawableList {
public:
  static DrawableListPtr Create(ContextWeak& aContext);

  void Reset();
  void AddDrawable(DrawablePtr&& aDrawable, const Matrix& aTransform);
  void Draw(const Camera& aCamera);

protected:
  struct State;
  DrawableList(State& aState, ContextWeak& aContext);
  ~DrawableList();

private:
  State& m;
  DrawableList() = delete;
  VRB_NO_DEFAULTS(DrawableList)
};

} // namespace vrb

#endif //  VRB_DRAWABLE_LIST_DOT_H
