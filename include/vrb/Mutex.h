/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_MUTEX_DOT_H
#define VRB_MUTEX_DOT_H

#include "vrb/MacroUtils.h"

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
private:
  VRB_NO_DEFAULTS(Mutex)
};

class MutexAutoLock {
public:
  MutexAutoLock(Mutex& aMutex) : mMutex(aMutex) {
    mMutex.Lock();
  }
  ~MutexAutoLock() {
    mMutex.Unlock();
  }
private:
  Mutex& mMutex;
  MutexAutoLock() = delete;
  VRB_NO_DEFAULTS(MutexAutoLock)
  VRB_NO_NEW_DELETE
};

class MutexAutoUnlock {
public:
  MutexAutoUnlock(Mutex& aMutex) : mMutex(aMutex) {
    mMutex.Unlock();
  }
  ~MutexAutoUnlock() {
    mMutex.Lock();
  }
private:
  Mutex& mMutex;
  MutexAutoUnlock() = delete;
  VRB_NO_DEFAULTS(MutexAutoUnlock)
  VRB_NO_NEW_DELETE
};

} // namespace vrb

#endif //  VRB_MUTEX_DOT_H
