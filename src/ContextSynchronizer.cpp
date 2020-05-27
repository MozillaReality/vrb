/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/ContextSynchronizer.h"
#include "vrb/ConcreteClass.h"
#include "vrb/ConditionVariable.h"
#include "vrb/Logger.h"
#include "vrb/RenderContext.h"
#include "vrb/ThreadTest.h"

#include "vrb/private/ResourceGLState.h"
#include "vrb/private/UpdatableState.h"

#include <pthread.h>
#include <algorithm>
#include <vector>

#define ASSERT_ON_CREATION_THREAD()                                          \
  if (!m.IsOnCreationThread()) {                                             \
    VRB_ERROR("ContextSynchronizer::%s called on wrong thread", __FUNCTION__); \
    return;                                                                  \
  }

namespace vrb {

struct ContextSynchronizer::State {
  RenderContextWeak renderContext;
  ThreadTestPtr renderThread;
  ThreadTestPtr threadSelf;
  Mutex observerLock;
  std::vector<ContextSynchronizerObserverPtr> observers;
  bool onRenderThread;
  ConditionVariable cond;
  bool active;
  bool waiting;
  ResourceGLList* uninitializedResources;
  ResourceGLList* resources;
  UpdatableList* updatables;

  State()
      : threadSelf(0)
      , onRenderThread(false)
      , active(true)
      , waiting(false)
      , uninitializedResources(nullptr)
      , resources(nullptr)
      , updatables(nullptr)
  {}
  bool IsOnCreationThread() {
    return threadSelf->IsOnInitializationThread();
  }
};

ContextSynchronizerPtr
ContextSynchronizer::Create(RenderContextPtr& aContext) {
  ContextSynchronizerPtr result = std::make_shared<ConcreteClass<ContextSynchronizer, ContextSynchronizer::State> >();
  result->m.renderThread = aContext->GetRenderThreadTest();
  result->m.renderContext = aContext;
  aContext->RegisterContextSynchronizer(result);
  return result;
}

void
ContextSynchronizer::BindToThread() {
  m.onRenderThread = m.renderThread->IsOnInitializationThread();
  m.threadSelf = ThreadTest::Create();
}

void
ContextSynchronizer::RegisterObserver(ContextSynchronizerObserverPtr& aObserver) {
  MutexAutoLock lock(m.observerLock);
  m.observers.push_back(aObserver);
}

void
ContextSynchronizer::ReleaseObserver(ContextSynchronizerObserverPtr& aObserver) {
  MutexAutoLock lock(m.observerLock);
  m.observers.erase(std::remove_if(m.observers.begin(), m.observers.end(),
                           [&aObserver](const ContextSynchronizerObserverPtr& value){ return aObserver.get() == value.get(); }),
                    m.observers.end());
}

void
ContextSynchronizer::AdoptLists(
    ResourceGLList& aUninitializedResources,
    ResourceGLList& aResources,
    UpdatableList& aUpdatables) {
  ASSERT_ON_CREATION_THREAD();
  if (!m.renderThread) {
    VRB_ERROR("ContextSynchronizer failed, no RenderContext defined");
    return;
  }
  if (m.renderThread->IsOnInitializationThread()) {
    RenderContextPtr context = m.renderContext.lock();
    if (!context) {
      return;
    }
    if (aUninitializedResources.IsDirty()) {
      context->GetUninitializedResourceGLList().AppendAndAdoptList(aUninitializedResources);
    }
    if (aResources.IsDirty()) {
      context->GetResourceGLList().AppendAndAdoptList(aResources);
    }
    if (aUpdatables.IsDirty()) {
      context->GetUpdatableList().AppendAndAdoptList(aUpdatables);
    }
  } else {
    MutexAutoLock lock(m.cond);
    if (aUninitializedResources.IsDirty()) {
      m.uninitializedResources = &aUninitializedResources;
    }
    if (aResources.IsDirty()) {
      m.resources = &aResources;
    }
    if (aUpdatables.IsDirty()) {
      m.updatables = &aUpdatables;
    }
    if (m.uninitializedResources|| m.resources || m.updatables) {
      m.waiting = true;
      while (m.waiting) {
        if (!m.cond.Wait()) {
          m.waiting = false;
          VRB_WARN("Condition variable Wait failed in ContextSynchronizer");
        }
      }
      m.uninitializedResources = nullptr;
      m.resources = nullptr;
      m.updatables = nullptr;
    }
  }
}

void
ContextSynchronizer::Signal(bool& aIsActive) {
  if (m.onRenderThread) {
    aIsActive = m.active;
    return;
  }
  if (!m.renderThread) {
    VRB_ERROR("ContextSynchronizer::Signal() failed. RenderContext not defined.");
    aIsActive = false;
    return;
  }
  if (!m.renderThread->IsOnInitializationThread()) {
    VRB_ERROR("ContextSynchronizer::Signal() failed. Must be called on main render thread");
    aIsActive = false;
    return;
  }
  MutexAutoLock lock(m.cond);
  aIsActive = m.active;
  if (m.waiting) {
    m.waiting = false;
    RenderContextPtr context = m.renderContext.lock();
    if (!context) {
      return;
    }
    if (m.uninitializedResources) {
      context->GetUninitializedResourceGLList().AppendAndAdoptList(*m.uninitializedResources);
    }
    if (m.resources) {
      context->GetResourceGLList().AppendAndAdoptList(*m.resources);
    }
    if (m.updatables) {
      context->GetUpdatableList().AppendAndAdoptList(*m.updatables);
    }
    {
      MutexAutoLock observerLock(m.observerLock);

      for (ContextSynchronizerObserverPtr& observer: m.observers) {
        observer->ContextsSynchronized(context);
      }
    }
    m.cond.Signal();
  }
}

void
ContextSynchronizer::Release() {
  MutexAutoLock lock(m.cond);
  m.active = false;
}

ContextSynchronizer::ContextSynchronizer(State& aState) : m(aState) {}
ContextSynchronizer::~ContextSynchronizer() {}

} // namespace vrb

