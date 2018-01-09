#ifndef VRB_GROUP_STATE_DOT_H
#define VRB_GROUP_STATE_DOT_H

#include "vrb/Forward.h"
#include "vrb/private/NodeState.h"
#include <vector>

namespace vrb {

struct Group::State : public Node::State {
  std::vector<NodePtr> children;
  bool Contains(const NodePtr& aNode);
};

}

#endif // VRB_GROUP_STATE_DOT_H
