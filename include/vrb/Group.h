/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
  virtual void RemoveNode(Node& aNode);
  void InsertNode(NodePtr aNode, uint32_t aIndex);
  const NodePtr& GetNode(uint32_t aIndex) const;
  int32_t GetNodeCount() const;
  void SortNodes(const std::function<bool(const vrb::NodePtr&, const vrb::NodePtr&)>& aFunction);

protected:
  bool Traverse(const GroupPtr& aParent, const Node::TraverseFunction& aTraverseFunction) override;
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
