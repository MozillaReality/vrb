/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_MUTEX_DOT_H
#define VRB_MUTEX_DOT_H

#include <pthread.h>

namespace vrb {

class Mutex {
public:
  Mutex() {
    pthread_mutex_init(&mMutex, nullptr);
  }
  ~Mutex() {
    pthread_mutex_destroy(&mMutex);
  }
  bool Lock() {
    return pthread_mutex_lock(&mMutex) == 0;
  }
  bool TryLock() {
    return pthread_mutex_trylock(&mMutex) == 0;
  }
  bool Unlock() {
    return pthread_mutex_unlock(&mMutex) == 0;
  }
protected:
  pthread_mutex_t mMutex;
};

} // namespace vrb

#endif //  VRB_MUTEX_DOT_H
