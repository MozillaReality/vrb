#ifndef VRB_CULL_VISITOR_STATE_DOT_H
#define VRB_CULL_VISITOR_STATE_DOT_H

#include "vrb/CullVisitor.h"
#include "vrb/Matrix.h"

namespace vrb {

struct CullVisitor::State {
  struct TransformNode {
    TransformNode* prev;
    Matrix transform;
    TransformNode() : prev(nullptr), transform(Matrix::Identity()) {}
  };

  const Matrix identity;
  TransformNode* transformList;

  State() : identity(Matrix::Identity()), transformList(nullptr) {}
  ~State() { Reset(); }
  void Reset();
};

} // namespace vrb

#endif //  VRB_CULL_VISITOR_STATE_DOT_H
