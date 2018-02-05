/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_UPDATABLE_STATE_DOT_H
#define VRB_UPDATABLE_STATE_DOT_H

#include "vrb/Updatable.h"

namespace vrb {

struct Updatable::State {
  Updatable* prevUpdatable;
  Updatable* nextUpdatable;

  State() : prevUpdatable(nullptr), nextUpdatable(nullptr) {}
  ~State() {
    if (prevUpdatable) { prevUpdatable->m.nextUpdatable = nextUpdatable; }
    if (nextUpdatable) { nextUpdatable->m.prevUpdatable = prevUpdatable; }
  }
  void CallAllUpdateResources(Context& aContext) {
    Updatable* current = nextUpdatable;
    while(current) {
      Updatable* tmp = current;
      current = current->m.nextUpdatable;
      tmp->UpdateResource(aContext);
    }
  }

  void Prepend(Updatable* aUpdatable) {
    aUpdatable->m.nextUpdatable = prevUpdatable->m.nextUpdatable;
    aUpdatable->m.prevUpdatable = prevUpdatable;
    prevUpdatable->m.nextUpdatable = aUpdatable;
    prevUpdatable = aUpdatable;
  }
};

} // namespace vrb

#endif // VRB_UPDATABLE_STATE_DOT_H
