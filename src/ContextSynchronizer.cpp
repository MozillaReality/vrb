/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/ContextSynchronizer.h"
#include "vrb/ConcreteClass.h"
#include "vrb/ConditionVariable.h"
#include "vrb/Logger.h"
#include "vrb/RenderContext.h"

#include "vrb/private/ResourceGLState.h"
#include "vrb/private/UpdatableState.h"

#include <pthread.h>
#include <vector>

#define ASSERT_ON_CREATION_THREAD()                                          \
  if (!m.IsOnCreationThread()) {                                             \
    VRB_ERROR("ContextSynchronizer::%s called on wrong thread", __FUNCTION__); \
    return;                                                                  \
  }

namespace vrb {

struct ContextSynchronizer::State {
  pthread_t threadSelf;
  Mutex observerLock;
  std::vector<ContextSynchronizerObserverPtr> observers;
  bool onRenderThread;
  RenderContextPtr context;
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
    return pthread_equal(threadSelf, pthread_self()) > 0;
  }
};

ContextSynchronizerPtr
ContextSynchronizer::Create(RenderContextPtr& aContext) {
  ContextSynchronizerPtr result = std::make_shared<ConcreteClass<ContextSynchronizer, ContextSynchronizer::State> >();
  result->m.context = aContext;
  aContext->RegisterContextSynchronizer(result);
  return result;
}

void
ContextSynchronizer::BindToThread() {
  m.onRenderThread = m.context->IsOnRenderThread();
  m.threadSelf = pthread_self();
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
  if (!m.context) {
    VRB_ERROR("ContextSynchronizer failed, no RenderContext defined");
    return;
  }
  if (m.context->IsOnRenderThread()) {
    if (aUninitializedResources.IsDirty()) {
      m.context->GetUninitializedResourceGLList().AppendAndAdoptList(aUninitializedResources);
    }
    if (aResources.IsDirty()) {
      m.context->GetResourceGLList().AppendAndAdoptList(aResources);
    }
    if (aUpdatables.IsDirty()) {
      m.context->GetUpdatableList().AppendAndAdoptList(aUpdatables);
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
  if (!m.context) {
    VRB_ERROR("ContextSynchronizer::Signal() failed. RenderContext not defined.");
    aIsActive = false;
    return;
  }
  if (!m.context->IsOnRenderThread()) {
    VRB_ERROR("ContextSynchronizer::Signal() failed. Must be called on main render thread");
    aIsActive = false;
    return;
  }
  MutexAutoLock lock(m.cond);
  aIsActive = m.active;
  if (m.waiting) {
    m.waiting = false;
    if (m.uninitializedResources) {
      m.context->GetUninitializedResourceGLList().AppendAndAdoptList(*m.uninitializedResources);
    }
    if (m.resources) {
      m.context->GetResourceGLList().AppendAndAdoptList(*m.resources);
    }
    if (m.updatables) {
      m.context->GetUpdatableList().AppendAndAdoptList(*m.updatables);
    }
    {
      MutexAutoLock observerLock(m.observerLock);

      for (ContextSynchronizerObserverPtr& observer: m.observers) {
        observer->ContextsSynchronized(m.context);
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

