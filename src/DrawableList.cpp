#include "vrb/DrawableList.h"
#include "vrb/private/DrawableListState.h"

#include "vrb/Camera.h"
#include "vrb/ConcreteClass.h"
#include "vrb/Drawable.h"

namespace vrb {

void
DrawableList::State::Reset() {
  DrawNode* current = list;
  list = nullptr;
  while (current) {
    DrawNode* tmp = current;
    current = current->next;
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
DrawableList::AddDrawable(DrawablePtr&& aDrawable, const Matrix& aTransform) {
  State::DrawNode* node = new State::DrawNode;
  node->drawable = aDrawable;
  node->transform = aTransform;
  node->next = m.list;
  m.list = node;
}

void
DrawableList::Draw(const Camera& aCamera) {
  State::DrawNode* current = m.list;
  while (current) {
    current->drawable->Draw(aCamera, current->transform);
    current = current->next;
  }
}

DrawableList::DrawableList(State& aState, ContextWeak& aContext) : m(aState) {}
DrawableList::~DrawableList() {}

} // namespace vrb
