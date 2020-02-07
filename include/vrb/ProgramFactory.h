/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_PROGRAM_FACTORY_DOT_H
#define VRB_PROGRAM_FACTORY_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

const uint32_t FeatureTexture = 0x1;
const uint32_t FeatureCubeTexture = 0x1 << 1;
const uint32_t FeatureSurfaceTexture = 0x1 << 2;
const uint32_t FeatureUVTransform = 0x1 << 3;
const uint32_t FeatureVertexColor = 0x1 << 4;
// Defaults to medium precision
const uint32_t FeatureHighPrecision = 0x01 << 5;
const uint32_t FeatureLowPrecision = 0x01 << 6;


class ProgramFactory {
public:
  static ProgramFactoryPtr Create();
  void SetLoaderThread(LoaderThreadPtr aLoader);
  ProgramPtr CreateProgram(CreationContextPtr& aContext, const uint32_t aFeatureMask);
  ProgramPtr CreateProgram(CreationContextPtr& aContext, const uint32_t aFeatureMask, const std::string& aCustomFragShader);
protected:
  struct State;
  ProgramFactory(State& aState);
private:
  State& m;
  ProgramFactory() = delete;
  VRB_NO_DEFAULTS(ProgramFactory)
};

} // namespace vrb

#endif // VRB_PROGRAM_FACTORY_DOT_H
