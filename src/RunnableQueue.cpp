/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/RunnableQueue.h"

#include "vrb/ConcreteClass.h"
#include "vrb/Logger.h"
#include "vrb/Mutex.h"

#include <jni.h>
#include <vector>

namespace {

} // namespace

namespace vrb {

struct RunnableQueue::State {
  Mutex lock;
  JavaVM* vm;
  JNIEnv* threadEnv;
  jmethodID runMethod;
  std::vector<jobject> list[2];
  uint32_t count;
  State()
      : vm(nullptr)
      , threadEnv(nullptr)
      , runMethod(nullptr)
      , count(0)
  {}
  ~State() {
    Shutdown();
  }
  void Initialize(JNIEnv* aEnv) {
    jclass localRunnableClass = aEnv->FindClass("java/lang/Runnable");
    if (!localRunnableClass) {
      VRB_ERROR("Failed to fine java class java/lang/Runnable in %s", __FILE__);
      return;
    }
    runMethod = aEnv->GetMethodID(localRunnableClass, "run", "()V");
    if (!runMethod) {
      VRB_ERROR("Failed to fine java/lang/Runnable.run() in %s", __FILE__);
    }
    aEnv->DeleteLocalRef(localRunnableClass);
    threadEnv = aEnv;
  }
  uint32_t AddPlace() {
    return ~count & 0x01;
  }
  uint32_t ProcessPlace() {
    return count & 0x01;
  }
  void Shutdown() {
    if (!vm) { return; }
    JNIEnv* env = nullptr;
    if (!vm->AttachCurrentThread(&env, nullptr)) { return; }
    for (int ix = 0; ix < 2; ix++) {
      for (jobject& runnable: list[ix]) {
        env->DeleteGlobalRef(runnable);
        runnable = nullptr;
      }
      list[ix].clear();
    }
    threadEnv = nullptr;
  }
};

RunnableQueuePtr
RunnableQueue::Create(JavaVM* aVM) {
  RunnableQueuePtr result = std::make_shared<ConcreteClass<RunnableQueue, RunnableQueue::State> >();
  result->m.vm = aVM;
  JNIEnv* env = nullptr;
  if (aVM->AttachCurrentThread(&env, nullptr) != 0) {
    VRB_ERROR("Failed to attach to current thread in %s:%s:%d", __FILE__, __FUNCTION__, __LINE__);
    return nullptr;
  }
  result->m.Initialize(env);
  return result;
}

bool
RunnableQueue::AttachToThread() {
  if (!m.vm) {
    return false;
  }

  JNIEnv* env = nullptr;
  if (m.vm->AttachCurrentThread(&env, nullptr) != 0) {
    return false;
  }
  m.Initialize(env);
  return true;
}

void
RunnableQueue::Clear() {
  m.Shutdown();
}

void
RunnableQueue::AddRunnable(JNIEnv* aEnv, jobject aRunnable) {
  jobject runnable = aEnv->NewGlobalRef(aRunnable);
  MutexAutoLock lock(m.lock);
  m.list[m.AddPlace()].push_back(runnable);
}

void
RunnableQueue::ProcessRunnables() {
  if (!m.vm) {
    VRB_ERROR("Unable to process Runnables, JavaVM is a nullptr in %s", __FILE__);
    return;
  }
  if (!m.threadEnv) {
    if (!AttachToThread()) {
      return;
    }
  }
  if (!m.runMethod) {
    VRB_ERROR("Unable to process Runnables, runMethod is a nullptr in %s", __FILE__);
    return;
  }
  {
    MutexAutoLock lock(m.lock);
    // Switch lists
    m.count++;
  }
  const uint32_t place = m.ProcessPlace();
  for (jobject& runnable: m.list[place]) {
    m.threadEnv->CallVoidMethod(runnable, m.runMethod);
    m.threadEnv->DeleteGlobalRef(runnable);
    runnable = nullptr;
  }
  m.list[place].clear();
}

RunnableQueue::RunnableQueue(State& aState) : m(aState) {}

} // namespace vrb
