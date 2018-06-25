/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_PROGRAM_FACTORY_DOT_H
#define VRB_PROGRAM_FACTORY_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class ProgramFactory {
public:
  static ProgramFactorPtr Create(CreationContextPtr& aContext);
  ProgramPtr CreatePhongProgram(const bool aEnableTexturing);
  // TODO: Add PBR Program.
protected:
  struct State;
  ProgramFactory(CreationContextPtr& aContext);
private:
  State& m;
  ProgramFactory() = delete;
  VRB_NO_DEFAULTS(ProgramFactory)
};

} // namespace vrb

#endif // VRB_PROGRAM_FACTORY_DOT_H
