/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_CONDITION_VARIABLE_DOT_H
#define VRB_CONDITION_VARIABLE_DOT_H

#include "vrb/Mutex.h"
#include "vrb/MacroUtils.h"

#include <pthread.h>

namespace vrb {

class ConditionVarible : public Mutex {
public:
  ConditionVarible() {
    pthread_cond_init(&mCond, nullptr);
  }
  ~ConditionVarible() {
    pthread_cond_destroy(&mCond);
  }
  bool Wait() {
    return pthread_cond_wait(&mCond, &mMutex) == 0;
  }
  bool Signal() {
    return pthread_cond_signal(&mCond) == 0;
  }
protected:
  pthread_cond_t mCond;
private:
  VRB_NO_DEFAULTS(ConditionVarible);
};

}

#endif // VRB_CONDITION_VARIABLE_DOT_H


