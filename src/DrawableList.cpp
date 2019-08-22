/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/DrawableList.h"
#include "vrb/private/DrawableListState.h"

#include "vrb/Camera.h"
#include "vrb/ConcreteClass.h"
#include "vrb/Drawable.h"
#include "vrb/RenderState.h"

namespace vrb {

void
DrawableList::State::Reset() {
  depth = 0;
  DrawNode* current = drawables;
  drawables = nullptr;
  while (current) {
    DrawNode* tmp = current;
    current = current->next;
    delete tmp;
  }
  LightSnapshot* currentLight = lights;
  lights = nullptr;
  while(currentLight) {
    LightSnapshot* tmp = currentLight;
    currentLight = currentLight->masterNext;
    delete tmp;
  }
}

DrawableListPtr
DrawableList::Create(CreationContextPtr& aContext) {
  return std::make_shared<ConcreteClass<DrawableList, DrawableList::State> >(aContext);
}

void
DrawableList::Reset() {
  m.Reset();
}

void
DrawableList::PushLight(const Light& aLight) {
  m.depth++;
  m.idCount++;
  if (m.idCount == 0) { m.idCount++; }
  State::LightSnapshot* light = new State::LightSnapshot(m.idCount, m.depth, aLight);
  light->next = m.currentLights;
  m.currentLights = light;
  light->masterNext = m.lights;
  m.lights = light;
}

void
DrawableList::PopLights(const int aCount) {
  for (int ix = 0; ix < aCount; ix++) {
    if (m.currentLights) {
      m.depth--;
      m.currentLights = m.currentLights->next;
    }
  }
  if (m.depth < 0) {
    VRB_ERROR("Light list depth in DrawableList is less than zero!");
  }
}

void
DrawableList::AddDrawable(DrawablePtr&& aDrawable, const Matrix& aTransform) {
  State::DrawNode* node = new State::DrawNode;
  node->drawable = aDrawable;
  node->transform = aTransform;
  node->lights = m.currentLights;
  node->next = m.drawables;
  m.drawables = node;
}

void
DrawableList::Draw(const Camera& aCamera) {
  State::DrawNode* current = m.drawables;
  while (current) {
    if (current->drawable->GetRenderState()) {
      const uint32_t id = current->lights ? current->lights->id : 0;
      if (id != current->drawable->GetRenderState()->GetLightId()) {
        current->drawable->GetRenderState()->ResetLights(id);
        State::LightSnapshot* snapshot = current->lights;
        while (snapshot) {
          current->drawable->GetRenderState()->AddLight(snapshot->direction, snapshot->ambient,
                                                        snapshot->diffuse, snapshot->specular);
          snapshot = snapshot->next;
        }
      }
    }
    current->drawable->Draw(aCamera, current->transform);
    current = current->next;
  }
}

DrawableList::DrawableList(State& aState, CreationContextPtr& aContext) : m(aState) {}
DrawableList::~DrawableList() {}

} // namespace vrb
