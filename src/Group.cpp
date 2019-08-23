/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/Group.h"
#include "vrb/private/GroupState.h"
#include "vrb/private/DrawableState.h"

#include "vrb/ConcreteClass.h"
#include "vrb/DrawableList.h"
#include "vrb/Light.h"
#include "vrb/Logger.h"
#include "vrb/Matrix.h"

#include <algorithm>
#include <memory>

namespace vrb {

class LambdaDrawable : public Drawable {
public:
  static LambdaDrawablePtr Create(CreationContextPtr& aContext, const RenderLambda& aLambda);
  RenderStatePtr& GetRenderState() override;
  void SetRenderState(const RenderStatePtr& aRenderState) override {}
  void Draw(const Camera& aCamera, const Matrix& aModelTransform) override;
protected:
  struct State;
  LambdaDrawable(State& aState, CreationContextPtr& aContext);
  ~LambdaDrawable() = default;
private:
  State& m;
};

struct LambdaDrawable::State : public Drawable::State {
  RenderLambda lambda;
  RenderStatePtr state;
};

LambdaDrawablePtr
LambdaDrawable::Create(CreationContextPtr& aContext, const RenderLambda& aLambda) {
  LambdaDrawablePtr result = std::make_shared<ConcreteClass<LambdaDrawable, LambdaDrawable::State> >(aContext);
  result->m.lambda = aLambda;
  return result;
}

RenderStatePtr&
LambdaDrawable::GetRenderState() {
  return m.state;
}


LambdaDrawable::LambdaDrawable(State& aState, CreationContextPtr& aContext) : m(aState), Drawable(aState, aContext) {}


void
LambdaDrawable::Draw(const Camera& aCamera, const Matrix& aModelTransform) {
  m.lambda();
}

LambdaDrawablePtr
Group::State::createLambdaDrawable(CreationContextPtr& aContext, const RenderLambda& aLambda) {
  if (aLambda) {
    return LambdaDrawable::Create(aContext, aLambda);
  }
  return nullptr;
}

bool
Group::State::Contains(const Node& aNode) {
  for (NodePtr& node: children) {
    if (node.get() == &aNode) {
      return true;
    }
  }
  return false;
}

bool
Group::State::Contains(const Light& aLight) {
  for (LightPtr& light: lights) {
    if (light.get() == &aLight) {
      return true;
    }
  }
  return false;
}

GroupPtr
Group::Create(CreationContextPtr& aContext) {
  GroupPtr group = std::make_shared<ConcreteClass<Group, Group::State> >(aContext);
  group->m.self = group;
  return group;
}

void
Group::Cull(CullVisitor& aVisitor, DrawableList& aDrawables) {
  for (LightPtr& light: m.lights) {
    aDrawables.PushLight(*light);
  }
  // Lambdas are added post first and pre last because the DrawablesList is FILO.
  if (m.postRenderLambda) {
    aDrawables.AddDrawable(m.postRenderLambda, Matrix());
  }
  for (NodePtr& node: m.children) {
    if (m.IsEnabled(*node)) {
      node->Cull(aVisitor, aDrawables);
    }
  }
  if (m.preRenderLambda) {
    aDrawables.AddDrawable(m.preRenderLambda, Matrix());
  }
  aDrawables.PopLights(m.lights.size());
}

void
Group::AddLight(LightPtr aLight) {
  if (!m.Contains(*aLight)) {
    m.lights.push_back(std::move(aLight));
  }
}

void
Group::RemoveLight(const Light& aLight) {
  for (auto it = m.lights.begin(); it != m.lights.end(); it++) {
    if (it->get() == &aLight) {
      m.lights.erase(it);
      return;
    }
  }
}

void
Group::AddNode(NodePtr aNode) {
  if (!m.Contains(*aNode)) {
    AddToParents(m.self, *aNode);
    m.children.push_back(std::move(aNode));
  }
}

void
Group::RemoveNode(Node& aNode) {
  for (auto childIt = m.children.begin(); childIt != m.children.end(); childIt++) {
    if (childIt->get() == &aNode) {
      m.children.erase(childIt);
      RemoveFromParents(*this, aNode);
      return;
    }
  }
}

void
Group::InsertNode(NodePtr aNode, uint32_t aIndex) {
  if (!m.Contains(*aNode)) {
    AddToParents(m.self, *aNode);
    m.children.insert(m.children.begin() + aIndex, std::move(aNode));
  }
}

const NodePtr&
Group::GetNode(uint32_t aIndex) const {
  return m.children.at(aIndex);
}

int32_t
Group::GetNodeCount() const {
  return m.children.size();
}

void
Group::SortNodes(const std::function<bool(const vrb::NodePtr&, const vrb::NodePtr&)>& aFunction) {
  std::sort(m.children.begin(), m.children.end(), aFunction);
}

void
Group::TakeChildren(GroupPtr& aSource) {
  for (NodePtr& child: aSource->m.children) {
    m.children.push_back(child);
  }
  aSource->m.Clear();
}

void
Group::SetPreRenderLambda(CreationContextPtr& aContext, const RenderLambda& aLambda) {
  m.preRenderLambda = m.createLambdaDrawable(aContext, aLambda);
}

void
Group::SetPostRenderLambda(CreationContextPtr& aContext, const RenderLambda& aLambda) {
  m.postRenderLambda = m.createLambdaDrawable(aContext, aLambda);
}

bool
Group::Traverse(const GroupPtr& aParent, const Node::TraverseFunction& aTraverseFunction) {
  for (NodePtr& child: m.children) {
    if (aTraverseFunction(child, aParent)) {
      return true;
    }
  }

  return false;
}

Group::Group(State& aState, CreationContextPtr& aContext) : Node(aState, aContext), m(aState) {}
Group::~Group() {
  for (NodePtr& child: m.children) {
    RemoveFromParents(*this, *child);
  }
}

}
