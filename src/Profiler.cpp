/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/Profiler.h"

#include "vrb/Logger.h"

#include <algorithm>
#include <limits>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace vrb {

struct Profiler::Sample {
  const std::string name;
  double startTime;
  double totalTime;
  double sampleCount;
  double longest;
  double shortest;

  Sample(const std::string aName) : name(aName) {
    Reset();
  }

  void Reset() {
    startTime = -1.0;
    totalTime = 0.0;
    sampleCount = 0.0;
    longest = -1.0;
    shortest = std::numeric_limits<double>::max();
  }

  // Not used
  Sample(const Sample&) = delete;
  Sample() = delete;
  Sample& operator=(const Sample&) = delete;
};

} // namespace vrb

namespace {

const double kNanosecondsToSeconds = 1.0e9;

struct ProfilerState {
   std::unordered_map<std::string, vrb::Profiler::Sample*> samples;
   std::vector<vrb::Profiler::Sample*> samplesList;
   vrb::Profiler::Sample frameSample;
   double lastDump = -1.0;

   ProfilerState() : frameSample("Frame") {}

   // Not used
   ProfilerState(const ProfilerState&) = delete;
   ProfilerState& operator= (const ProfilerState&) = delete;
};

ProfilerState* sState;

void
Initialize() {
  if (sState) {
    return;
  }
  sState = new ProfilerState;
}

double
GetTime() {
  double result = -1.0;
  timespec spec = {};
  if (clock_gettime(CLOCK_MONOTONIC, &spec) == 0) {
    result = (double)spec.tv_sec + (spec.tv_nsec / kNanosecondsToSeconds);
  }
  return result;
}

void PrintSample(const vrb::Profiler::Sample& aSample) {
  std::stringstream stream;
  stream << aSample.name;
  if (aSample.sampleCount <= 0.0) {
    stream << ": No samples";
  } else {
    stream << ": Average: " << aSample.totalTime / aSample.sampleCount
           << " Shortest: " << aSample.shortest
           << " Longest: " << aSample.longest
           << " Samples: " << aSample.sampleCount;
  }
  VRB_ERROR("%s", stream.str().c_str());
}

void
DumpState() {
  if (!sState) {
    VRB_ERROR("VRB Profiler not initialized");
    return;
  }
  std::sort(sState->samplesList.begin(), sState->samplesList.end(), [](const vrb::Profiler::Sample* a, const vrb::Profiler::Sample* b) -> bool {
    if (!a || !b) {
      return b == nullptr;
    }
    if (b->sampleCount <= 0.0) {
      return true;
    } else if (a->sampleCount <= 0.0f) {
      return false;
    }
    return (a->totalTime / a->sampleCount) > (b->totalTime / b->sampleCount);
  });
  VRB_ERROR("= Profiler ==============================================================");
  PrintSample(sState->frameSample);
  sState->frameSample.Reset();
  for (auto sample = sState->samplesList.begin(); sample != sState->samplesList.end(); sample++) {
    PrintSample(*(*sample));
    (*sample)->Reset();
  }
  VRB_ERROR("=========================================================================");
}

} // namespace

namespace vrb {

Profiler::Sample*
Profiler::CreateSample(const std::string& aSampleName) {
  Initialize();
  if (sState->samples.find(aSampleName) != sState->samples.end()) {
    return sState->samples.at(aSampleName);
  }
  Sample* sample = new Sample(aSampleName);
  sState->samples[aSampleName] = sample;
  sState->samplesList.push_back(sample);
  return sample;
}

void
Profiler::StartFrame() {
  if (!sState) {
    Initialize();
  }
  StartSample(&(sState->frameSample));
}

void
Profiler::StartSample(Sample* aSample) {
  if (!sState || !aSample) {
    return;
  }

  aSample->sampleCount++;
  aSample->startTime = GetTime();
}

void
Profiler::EndSample(Sample* aSample) {
  if (!sState || !aSample) {
    return;
  }
  const double ctime = GetTime();
  const double delta = ctime - aSample->startTime;
  if (delta > aSample->longest) {
    aSample->longest = delta;
  }
  if (delta < aSample->shortest) {
    aSample->shortest = delta;
  }
  aSample->totalTime += delta;
}

void
Profiler::EndFrame() {
  if (!sState) {
    return;
  }
  EndSample(&(sState->frameSample));
  const double ctime = GetTime();
  if (sState->lastDump < 0.0) {
    sState->lastDump = ctime;
    return;
  }
  static const double kDumpTime = 2.0;
  if ((ctime - sState->lastDump > kDumpTime)) {
    DumpState();
    sState->lastDump = ctime;
  }
}

} // namespace vrb

