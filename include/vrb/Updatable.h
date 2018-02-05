/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_UPDATABLE_DOT_H
#define VRB_UPDATABLE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class Updatable {
public:
  virtual void UpdateResource(Context& aContext) = 0;
protected:
  struct State;
  Updatable(State& aState, ContextWeak& aContext);
  Updatable(State& aState);
  virtual ~Updatable();
private:
  State& m;
  Updatable() = delete;
  VRB_NO_DEFAULTS(Updatable)
};

} // namespace vrb

#endif //  VRB_UPDATABLE_DOT_H
