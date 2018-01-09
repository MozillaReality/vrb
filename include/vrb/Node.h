#ifndef VRB_NODE_DOT_H
#define VRB_NODE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include <string>

namespace vrb {

class Node {
public:
  const std::string& GetName() const;
  void SetName(const std::string& aName);
  virtual void Cull(CullVisitorPtr& aVisitor, DrawableListPtr& aDrawables) = 0;
protected:
  struct State;
  Node(State& aState, ContextWeak& aContext);
  virtual ~Node();
private:
  State& m;
  Node() = delete;
  VRB_NO_DEFAULTS(Node)
};

} // namespace vrb

#endif // VRB_NODE_DOT_H
