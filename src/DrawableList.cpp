#include "vrb/DrawableList.h"
#include "vrb/private/DrawableListState.h"

#include "vrb/Camera.h"
#include "vrb/ConcreteClass.h"
#include "vrb/Drawable.h"

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
DrawableList::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<DrawableList, DrawableList::State> >(aContext);
}

void
DrawableList::Reset() {
  m.Reset();
}

void
DrawableList::PushLight(const Light& aLight) {
  m.depth++;
  State::LightSnapshot* light = new State::LightSnapshot(m.depth, aLight);
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
    VRB_LOG("ERROR: Light list depth in DrawableList is less than zero!");
  }
}

void
DrawableList::AddDrawable(DrawablePtr&& aDrawable, const Matrix& aTransform) {
  State::DrawNode* node = new State::DrawNode;
  node->drawable = aDrawable;
  node->transform = aTransform;
  node->next = m.drawables;
  m.drawables = node;
}

void
DrawableList::Draw(const Camera& aCamera) {
  State::DrawNode* current = m.drawables;
  while (current) {
    current->drawable->Draw(aCamera, current->transform);
    current = current->next;
  }
}

DrawableList::DrawableList(State& aState, ContextWeak& aContext) : m(aState) {}
DrawableList::~DrawableList() {}

} // namespace vrb
