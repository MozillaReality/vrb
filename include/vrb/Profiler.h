/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_PROFILER_DOT_H
#define VRB_PROFILER_DOT_H

#include <string>

namespace vrb {

class Profiler {
public:
  struct Sample;
  static Sample* CreateSample(const std::string& aSampleName);
  static void StartFrame();
  static void StartSample(Sample* aSample);
  static void EndSample(Sample* aSample);
  static void EndFrame();
};

#define VRB_START_SAMPLE(name) \
{ \
  static vrb::Profiler::Sample* sample = vrb::Profiler::CreateSample(name); \
  vrb::Profiler::StartSample(sample); \
}

#define VRB_END_SAMPLE(name) \
{ \
  static vrb::Profiler::Sample* sample = vrb::Profiler::CreateSample(name); \
  vrb::Profiler::EndSample(sample); \
}

}

#endif //  VRB_PROFILER_DOT_H
