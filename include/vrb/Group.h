#ifndef VRB_GROUP_DOT_H
#define VRB_GROUP_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/Node.h"

namespace vrb {

class Group : public Node {
public:
  static GroupPtr Create(ContextWeak& aContext);

  // Node interface
  void Cull(CullVisitor& aVisitor, DrawableList& aDrawables) override;

  // Group interface
  void AddLight(LightPtr aLight);
  void RemoveLight(const Light& aLight);
  void AddNode(NodePtr aNode);
  void RemoveNode(const Node& aNode);

protected:
  struct State;
  Group(State& aState, ContextWeak& aContext);
  ~Group();

private:
  State& m;
  Group() = delete;
  VRB_NO_DEFAULTS(Group)
};

} // namespace vrb

#endif // VRB_GROUP_DOT_H
