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
  void CallAllUpdateResources(RenderContext& aContext) {
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

  void PrependAndAdoptList(Updatable& self, Updatable& aHead, Updatable& aTail) {
    if (!prevUpdatable) {
      return;
    }

    if (!aHead.m.nextUpdatable) {
      return;
    }

    if (!aTail.m.prevUpdatable) {
      return;
    }

    prevUpdatable->m.nextUpdatable = aHead.m.nextUpdatable;
    aHead.m.nextUpdatable->m.prevUpdatable = prevUpdatable;
    prevUpdatable = aTail.m.prevUpdatable;
    aTail.m.prevUpdatable->m.nextUpdatable = &self;

    aTail.m.prevUpdatable = &aHead;
    aHead.m.nextUpdatable = &aTail;
  }
};

class UpdatableTail : public Updatable {
public:
  UpdatableTail() : Updatable(m) {}
  void UpdateResource(RenderContext&) override {} // noop

  void Prepend(vrb::Updatable* aUpdatable)  {
    m.Prepend(aUpdatable);
  }

  void PrependAndAdoptList(Updatable& aHead, Updatable& aTail)  {
    m.PrependAndAdoptList(*this, aHead, aTail);
  }

  void SetHead(Updatable* aHead) {
    m.prevUpdatable = aHead;
  }
protected:
  Updatable::State m;
};

class UpdatableList : public Updatable {
public:
  UpdatableList() : Updatable(m) {
    m.nextUpdatable = &mTail;
    mTail.SetHead(this);
  }
  ~UpdatableList() {}

  // vrb::Updatable interface
  void UpdateResource(RenderContext& aContext) override  {
    m.CallAllUpdateResources(aContext);
  }

  bool IsDirty() {
    return m.nextUpdatable != &mTail;
  }

  void Append(vrb::Updatable* aUpdatable)  {
    mTail.Prepend(aUpdatable);
  }

  void AppendAndAdoptList(UpdatableList& aList)  {
    mTail.PrependAndAdoptList(aList, aList.mTail);
  }
protected:
  UpdatableTail mTail;
  Updatable::State m;
};



} // namespace vrb

#endif // VRB_UPDATABLE_STATE_DOT_H
