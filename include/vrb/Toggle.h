/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_TOGGLE_DOT_H
#define VRB_TOGGLE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/Group.h"

namespace vrb {

class Toggle : public Group {
public:
  static TogglePtr Create(ContextWeak& aContext);

  // Group interface
  void RemoveNode(const Node& aNode) override;

  // Toggle interface
  void ToggleAll(const bool aEnabled);
  bool IsEnabled(const Node& aNode);
  void ToggleChild(const Node& aNode, const bool aEnabled);

protected:
  typedef Group Super;
  struct State;
  Toggle(State& aState, ContextWeak& aContext);
  ~Toggle();

private:
  State& m;
  Toggle() = delete;
  VRB_NO_DEFAULTS(Toggle)
};

} // namespace vrb

#endif // VRB_TOGGLE_DOT_H
