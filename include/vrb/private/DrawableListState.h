#ifndef VRB_DRAWABLE_LIST_STATE_DOT_H
#define VRB_DRAWABLE_LIST_STATE_DOT_H

#include "vrb/DrawableList.h"
#include "vrb/Matrix.h"

namespace vrb {

struct DrawableList::State {
  struct DrawNode {
    DrawNode* next;
    DrawablePtr drawable;
    Matrix transform;

    DrawNode() : next(nullptr) {}
  };

  DrawNode* list;

  State() : list(nullptr) {}
  ~State() { Reset(); }
  void Reset();
};

}

#endif // VRB_DRAWABLE_LIST_STATE_DOT_H
