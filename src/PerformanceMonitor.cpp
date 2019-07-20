/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/PerformanceMonitor.h"
#include "vrb/private/UpdatableState.h"
#include "vrb/ConcreteClass.h"
#include "vrb/Logger.h"
#include "vrb/RenderContext.h"

#include <array>
#include <cmath>
#include <forward_list>
#include <limits>

namespace {
const double kInvalidTimestamp = -1.0;
const double kResumePauseDuration = 3.0;
const int32_t kSampleCount = 5;
const int32_t kMinBadSampleCount = 2;
const double kMinFrameRate = 59.0;
const double kFrameEpsilon = 2.0;
const double kSampleTimeDelta = 1.0;
const double kMaxSampleTimeDelta = 1.1;
}

namespace vrb {

struct PerformanceMonitor::State : public Updatable::State {
  bool paused = false;
  bool slow = false;
  bool sampling = true;
  double minFrameRate = kMinFrameRate;
  double timeStamp = kInvalidTimestamp;
  double frameCount = 0.0;
  double resumePause = kInvalidTimestamp;
  std::array<double, kSampleCount> samples = {};
  int32_t samplePlace = 0;
  std::forward_list<PerformanceMonitorObserverPtr> observers;
  State() {
    Clear();
  }

  void Clear() {
    for (double& value: samples) {
      value = -1.0;
    }
    timeStamp = kInvalidTimestamp;
    frameCount = 0.0;
    resumePause = kInvalidTimestamp;
  }

  void Sample() {
    double min = std::numeric_limits<double>::max();
    double max = 0.0;
    double sum = 0.0;
    for (double& value: samples) {
      if (value < 0.0) {
        return;
      }
      sum += value;
      if (value > max) {
        max = value;
      } else if (value < min){
        min = value;
      }
    }
    if ((max - min) < kFrameEpsilon) {
      sampling = false;
      minFrameRate = std::floor((sum / kSampleCount) - 1.0);

      if (minFrameRate < kMinFrameRate) {
        VRB_ERROR("Minimum sampled framerate: %fHz is too low setting to %fHz", minFrameRate, kMinFrameRate);
        minFrameRate = kMinFrameRate;
      }
      VRB_LOG("Performance monitor setting minimum framerate to %fHz", minFrameRate);
    }
  }

  void Validate() {
    if (observers.empty()) {
      return;
    }
    if (sampling) {
      Sample();
      return;
    }
    int32_t badCount = 0;
    double totalCount = 0.0;
    double sum = 0.0;
    for (double& value: samples) {
      if (value > 0.0) {
        sum += value;
        totalCount++;
        if(value < minFrameRate) {
          badCount++;
        }
      }
    }
    const double averageFrameRate = sum / totalCount;
    if ((badCount >= kMinBadSampleCount) && !slow) {
      slow = true;
      VRB_ERROR("Render performance has dropped bellow acceptable levels.");
      for (PerformanceMonitorObserverPtr& observer: observers) {
        observer->PoorPerformanceDetected(minFrameRate, averageFrameRate);
      }
    } else if ((badCount == 0) && slow) {
      slow = false;
      VRB_LOG("Render performance has been restored.");
      for (PerformanceMonitorObserverPtr& observer: observers) {
        observer->PerformanceRestored(minFrameRate, averageFrameRate);
      }
    }
  }
};

PerformanceMonitorPtr
PerformanceMonitor::Create(CreationContextPtr& aContext) {
  return std::make_shared<ConcreteClass<PerformanceMonitor, PerformanceMonitor::State> >(aContext);
}

double
PerformanceMonitor::GetMinFrameRate() const {
  return m.minFrameRate;
}

void
PerformanceMonitor::Pause() {
  VRB_DEBUG("Pausing Performance Monitor");
  m.Clear();
  m.paused = true;
}

void
PerformanceMonitor::Resume() {
  VRB_DEBUG("Resuming Performance Monitor");
  m.paused = false;
}

void
PerformanceMonitor::Resample() {
  m.Clear();
  m.sampling = true;
}

void
PerformanceMonitor::AddPerformanceMonitorObserver(PerformanceMonitorObserverPtr aObserver) {
  m.observers.push_front(std::move(aObserver));
}

void
PerformanceMonitor::RemovePerformanceMonitorObserver(const vrb::PerformanceMonitorObserver& aObserver) {
  m.observers.remove_if([&](PerformanceMonitorObserverPtr& aObserverPtr) -> bool {
    return &aObserver == aObserverPtr.get();
  });
}

PerformanceMonitor::PerformanceMonitor(State& aState, CreationContextPtr& aContext)
    : Updatable(aState, aContext)
    , m(aState) {}

void
PerformanceMonitor::UpdateResource(RenderContext& aContext) {
  if (m.paused) {
    return;
  }
  const double ctime = aContext.GetTimestamp();
  if (m.resumePause < 0.0) {
    m.resumePause = ctime + kResumePauseDuration;
    return;
  } else if (m.resumePause > ctime) {
    return;
  }
  if (m.timeStamp <= 0.0) {
      m.timeStamp = ctime;
      m.frameCount++;
    return;
  }
  const double delta = ctime - m.timeStamp;
  if (delta > kMaxSampleTimeDelta) {
    VRB_DEBUG("Discarding sample, frame delta was too large: %f", delta);
    m.frameCount = 1.0;
    m.timeStamp = ctime;
  } else if (delta > kSampleTimeDelta) {
    m.samples[m.samplePlace] = m.frameCount / delta;
    VRB_DEBUG("Average Frame Rate: %.0fHz", std::round(m.samples[m.samplePlace]));
    m.samplePlace = (m.samplePlace + 1) % kSampleCount;
    m.frameCount = 1.0;
    m.timeStamp = ctime;
    m.Validate();
  } else {
    m.frameCount++;
  }
}

} // namespace vrb
