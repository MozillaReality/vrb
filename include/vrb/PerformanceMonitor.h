/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_PERFORMANCE_MONITOR_DOT_H
#define VRB_PERFORMANCE_MONITOR_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/Updatable.h"

#include <string>
#include <memory>

namespace vrb {

class PerformanceMonitorObserver {
public:
  virtual void PoorPerformanceDetected(const double& aTargetFrameRate, const double& aAverageFrameRate) = 0;
  virtual void PerformanceRestored(const double& aTargetFrameRate, const double& aAverageFrameRate) = 0;
protected:
  explicit PerformanceMonitorObserver() = default;
  ~PerformanceMonitorObserver() = default;
  VRB_NO_DEFAULTS(PerformanceMonitorObserver)
  VRB_NO_NEW_DELETE
};

class PerformanceMonitor : protected Updatable {
public:
  static PerformanceMonitorPtr Create(CreationContextPtr& aContext);
  double GetAverageFrameRate() const;
  double GetPerfomranceDelta() const;
  void SetPerformanceDelta(const double aDelta);
  void Pause();
  void Resume();
  void Resample();
  void AddPerformanceMonitorObserver(PerformanceMonitorObserverPtr aObserver);
  void RemovePerformanceMonitorObserver(const PerformanceMonitorObserver& aObserver);

protected:
  struct State;
  PerformanceMonitor(State& aState, CreationContextPtr& aContext);
  ~PerformanceMonitor() = default;

  // Updatable interface
  void UpdateResource(RenderContext& aContext) override;

private:
  VRB_NO_DEFAULTS(PerformanceMonitor)
  State& m;
};

class PausePerformanceMonitor {
private:
  PerformanceMonitor& mMonitor;
  VRB_NO_DEFAULTS(PausePerformanceMonitor)
  PausePerformanceMonitor() = delete;
public:
  PausePerformanceMonitor(PerformanceMonitor& aMonitor) : mMonitor(aMonitor) {
    mMonitor.Pause();
  }
  ~PausePerformanceMonitor() {
    mMonitor.Resume();
  }
};

}

#endif // VRB_PERFORMANCE_MONITOR_DOT_H
