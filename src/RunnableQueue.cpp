/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/RunnableQueue.h"

#include "vrb/Mutex.h"
#include "vrb/ConcreteClass.h"

#include <jni.h>
#include <vector>

namespace {

} // namespace

namespace vrb {

struct RunnableQueue::State {
  Mutex lock;
  JavaVM* vm;
  JNIEnv* processEnv;
  jclass runnableClass;
  jmethodID runMethod;
  std::vector<jobject> list[2];
  int add;
  int process;
  State()
      : vm(nullptr)
      , processEnv(nullptr)
      , runnableClass(nullptr)
      , runMethod(nullptr)
      , add(0)
      , process(1)
  {}
  ~State() {
    if (!vm) { return; }
    JNIEnv* env = nullptr;
    if (!vm->AttachCurrentThread(&env, nullptr)) { return; }
    if (runnableClass) {
      env->DeleteGlobalRef(runnableClass);
    }
    for (int ix = 0; ix < 2; ix++) {
      for (jobject& runnable: list[ix]) {
        env->DeleteGlobalRef(runnable);
        runnable = nullptr;
      }
      list[ix].clear();
    }
  }
  void Init(JavaVM* aVM) {
    vm = aVM;
    JNIEnv* env = nullptr;
    if (!vm->AttachCurrentThread(&env, nullptr)) { return; }
    jclass localRunnableClass = env->FindClass("java/lang/Runnable");
    runnableClass = (jclass)env->NewGlobalRef(localRunnableClass);
    runMethod = env->GetMethodID(runnableClass, "run", "()V");
  }
};

RunnableQueuePtr
RunnableQueue::Create(JavaVM* aVM) {
  RunnableQueuePtr result = std::make_shared<ConcreteClass<RunnableQueue, RunnableQueue::State> >();
  result->m.Init(aVM);
  return result;
}

void
RunnableQueue::AddRunnable(JNIEnv* aEnv, jobject aRunnable) {
  jobject runnable = aEnv->NewGlobalRef(aRunnable);
  MutexAutoLock lock(m.lock);
  m.list[m.add].push_back(runnable);
}

void
RunnableQueue::ProcessRunnables() {
  if (!m.vm || m.runMethod) { return; }
  if (!m.processEnv) {
    if (!m.vm->AttachCurrentThread(&(m.processEnv), nullptr)) { return; }
  }
  {
    MutexAutoLock lock(m.lock);
    // Switch lists
    m.add = m.add ^ 0x01;
    m.process = m.process ^ 0x01;
  }
  for (jobject& runnable: m.list[m.process]) {
    m.processEnv->CallVoidMethod(runnable, m.runMethod);
    m.processEnv->DeleteGlobalRef(runnable);
    runnable = nullptr;
  }
  m.list[m.process].clear();
}

RunnableQueue::RunnableQueue(State& aState) : m(aState) {}
RunnableQueue::~RunnableQueue() {

}

} // namespace vrb
