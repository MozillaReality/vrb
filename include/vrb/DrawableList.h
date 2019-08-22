/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_DRAWABLE_LIST_DOT_H
#define VRB_DRAWABLE_LIST_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include <functional>

namespace vrb {

class DrawableList {
public:
  static DrawableListPtr Create(CreationContextPtr& aContext);

  void Reset();
  void PushLight(const Light& aLight);
  void PopLights(const int aCount);
  void AddDrawable(DrawablePtr&& aDrawable, const Matrix& aTransform);
  using Lambda = std::function<void()>;
  void PushLambdas(const Lambda& aPreRender, const Lambda& aPostRender);
  void PopLambdas();
  void Draw(const Camera& aCamera);

protected:
  struct State;
  DrawableList(State& aState, CreationContextPtr& aContext);
  ~DrawableList();

private:
  State& m;
  DrawableList() = delete;
  VRB_NO_DEFAULTS(DrawableList)
};

} // namespace vrb

#endif //  VRB_DRAWABLE_LIST_DOT_H
