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
const double kDefaultPerformanceDelta = 10.0;
const int32_t kSampleCount = 5;
const int32_t kMinBadSampleCount = 2;
const double kMinAverageFrameRate = 60.0;
const double kFrameEpsilon = 2.0;
const double kSampleTimeDelta = 1.0;
const double kMaxSampleTimeDelta = 3.0;
}

namespace vrb {

struct PerformanceMonitor::State : public Updatable::State {
  bool paused = false;
  bool slow = false;
  double averageFrameRate = kMinAverageFrameRate;
  double timeStamp = kInvalidTimestamp;
  double frameCount = 0.0;
  double resumePause = kInvalidTimestamp;
  double performanceDelta = kDefaultPerformanceDelta;
  double startOfPoorPerformanceTime = kInvalidTimestamp;
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

  void Validate() {
    if (observers.empty()) {
      return;
    }

    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();
    int32_t badCount = 0;
    double badSum = 0.0;
    double totalCount = 0.0;
    double totalSum = 0.0;
    bool missingSamples = false;
    for (double& value: samples) {
      if (value > 0.0) {
        totalSum += value;
        if (value > max) {
          max = value;
        } else if (value < min) {
          min = value;
        }
        totalCount++;
        if (value < (averageFrameRate - performanceDelta)) {
          badSum += value;
          badCount++;
        }
      } else {
        missingSamples = true;
      }
    }
    if (((max - min) < kFrameEpsilon) && !missingSamples) {
      const double frameRate = std::floor(totalSum / kSampleCount);

      if (frameRate > averageFrameRate) {
        averageFrameRate = frameRate;
        VRB_DEBUG("Performance Monitor detected average frame rate %.0fHz", averageFrameRate);
        VRB_LOG("Performance monitor setting minimum frame rate to %.0fHz",
                averageFrameRate - performanceDelta);
      }
    }
    if ((badCount >= kMinBadSampleCount) && !slow) {
      startOfPoorPerformanceTime = timeStamp - (kSampleTimeDelta * kMinBadSampleCount);
      const double badFrameRate = badSum / badCount;
      slow = true;
      VRB_ERROR("Render performance has dropped bellow acceptable levels: %.0fHz", badFrameRate);
      for (PerformanceMonitorObserverPtr& observer: observers) {
        observer->PoorPerformanceDetected(averageFrameRate, badFrameRate);
      }
    } else if ((badCount == 0) && slow) {
      const double currentFrameRate = totalSum / totalCount;
      slow = false;
      VRB_LOG("Render performance has been restored: %.0fHz after %.1fsec", currentFrameRate, timeStamp - startOfPoorPerformanceTime - (kSampleTimeDelta * kSampleCount));
      for (PerformanceMonitorObserverPtr& observer: observers) {
        observer->PerformanceRestored(averageFrameRate, currentFrameRate);
      }
    }
  }
};

PerformanceMonitorPtr
PerformanceMonitor::Create(CreationContextPtr& aContext) {
  return std::make_shared<ConcreteClass<PerformanceMonitor, PerformanceMonitor::State> >(aContext);
}

double
PerformanceMonitor::GetAverageFrameRate() const {
  return m.averageFrameRate;
}

double
PerformanceMonitor::GetPerfomranceDelta() const {
  return m.performanceDelta;
}

void
PerformanceMonitor::SetPerformanceDelta(const double aDelta) {
  if (aDelta <= 0.0) {
    VRB_ERROR("Performance delta must be greater than zero.");
    return;
  }
  m.performanceDelta = aDelta;
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
  m.averageFrameRate = kMinAverageFrameRate;
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
    VRB_DEBUG("Discarding sample, frame delta was too large: %f sec", delta);
    m.frameCount = 1.0;
    m.timeStamp = ctime;
  } else if (delta >= kSampleTimeDelta) {
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
