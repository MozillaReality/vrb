#include "vrb/Node.h"
#include "vrb/private/NodeState.h"

namespace vrb {

const std::string&
Node::GetName() const { return m.name; }

void
Node::SetName(const std::string& aName) { m.name = aName; }

Node::Node(State& aState, ContextWeak& aContext) : m(aState) {}
Node::~Node() {}

}
