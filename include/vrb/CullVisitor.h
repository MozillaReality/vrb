/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_CULL_VISITOR_DOT_H
#define VRB_CULL_VISITOR_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class CullVisitor {
public:
  static CullVisitorPtr Create(CreationContextPtr& aContext);
  const Matrix& GetTransform() const;
  void PushTransform(const Matrix& aTransform);
  void PopTransform();

protected:
  struct State;
  CullVisitor(State& aState, CreationContextPtr& aContext);
  ~CullVisitor();

private:
  State& m;
  CullVisitor() = delete;
  VRB_NO_DEFAULTS(CullVisitor)
};

} // namespace vrb

#endif // VRB_CULL_VISITOR_DOT_H
