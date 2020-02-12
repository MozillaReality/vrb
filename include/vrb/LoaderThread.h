/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_LOADER_THREAD_DOT_H
#define VRB_LOADER_THREAD_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include <functional>
#include <string>

namespace vrb {

typedef std::function<void(GroupPtr&)> LoadFinishedCallback;
typedef std::function<GroupPtr(CreationContextPtr&)> LoadTask;

class LoaderThread {
public:
  virtual void RunLoadTask(GroupPtr aTargetNode, LoadTask& aTask) = 0;
  virtual void RunLoadTask(GroupPtr aTargetNode, LoadTask& aTask, LoadFinishedCallback& aCallback) = 0;
  virtual void AddFinishedCallback(LoadFinishedCallback& aCallback) = 0;
  virtual bool IsOnLoaderThread() const = 0;
protected:
  LoaderThread() = default;
  ~LoaderThread() = default;
private:
  VRB_NO_DEFAULTS(LoaderThread);
};

} // namespace vrb

#endif // VRB_LOADER_THREAD_DOT_H
