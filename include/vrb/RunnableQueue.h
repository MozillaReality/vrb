/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_RUNNABLE_QUEUE_DOT_H
#define VRB_RUNNABLE_QUEUE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include <jni.h>

namespace vrb {

class RunnableQueue {
public:
  RunnableQueuePtr Create(JavaVM* aVM);

  void AddRunnable(JNIEnv* aEnv, jobject aRunnable);
  void ProcessRunnables();
protected:
  struct State;
  RunnableQueue(State& aState);
  ~RunnableQueue();
private:
  State& m;
  RunnableQueue() = delete;
  VRB_NO_DEFAULTS(RunnableQueue)
};

}

#endif //  VRB_RUNNABLE_QUEUE_DOT_H
