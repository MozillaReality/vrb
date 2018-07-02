#include "vrb/ContextSynchronizer.h"
#include "vrb/ConcreteClass.h"
#include "vrb/ConditionVarible.h"
#include "vrb/Logger.h"
#include "vrb/RenderContext.h"

#include "vrb/private/ResourceGLState.h"
#include "vrb/private/UpdatableState.h"

#include <pthread.h>
#include <vector>

#define ASSERT_ON_CREATION_THREAD()                                          \
  if (!m.IsOnCreationThread()) {                                             \
    VRB_LOG("ContextSynchronizer::%s called on wrong thread", __FUNCTION__); \
    return;                                                                  \
  }

namespace vrb {

struct ContextSynchronizer::State {
  pthread_t threadSelf;
  Mutex observerLock;
  std::vector<ContextSynchronizerObserverPtr> observers;
  bool onRenderThread;
  RenderContextPtr context;
  ConditionVarible cond;
  bool active;
  bool waiting;
  ResourceGLHead* resourceHead;
  ResourceGLTail* resourceTail;
  UpdatableHead* updatableHead;
  UpdatableTail* updatableTail;

  State()
      : threadSelf(0)
      , onRenderThread(false)
      , active(true)
      , waiting(false)
      , resourceHead(nullptr)
      , resourceTail(nullptr)
      , updatableHead(nullptr)
      , updatableTail(nullptr)
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
  MutexAutoLock(m.observerLock);
  m.observers.push_back(aObserver);
}

void
ContextSynchronizer::ReleaseObserver(ContextSynchronizerObserverPtr& aObserver) {
  MutexAutoLock(m.observerLock);
  m.observers.erase(std::remove_if(m.observers.begin(), m.observers.end(),
                           [&aObserver](const ContextSynchronizerObserverPtr& value){ return aObserver.get() == value.get(); }),
                    m.observers.end());
}

void
ContextSynchronizer::AdoptLists(ResourceGLHead& aResourceHead, ResourceGLTail& aResourceTail,
                                UpdatableHead& aUpdatableHead, UpdatableTail& aUpdatableTail) {
  ASSERT_ON_CREATION_THREAD();
  if (!m.context) {
    VRB_LOG("ContextSynchronizer failed, no RenderContext defined");
    return;
  }
  if (m.context->IsOnRenderThread()) {
    if (aResourceHead.IsDirty(aResourceTail)) {
      m.context->GetResourceGLTail().PrependAndAdoptList(aResourceHead, aResourceTail);
    }
    if (aUpdatableHead.IsDirty(aUpdatableTail)) {
      m.context->GetUpdatableTail().PrependAndAdoptList(aUpdatableHead, aUpdatableTail);
    }
  } else {
    MutexAutoLock lock(m.cond);
    if (aResourceHead.IsDirty(aResourceTail)) {
      m.resourceHead = &aResourceHead;
      m.resourceTail = &aResourceTail;
    }
    if (aUpdatableHead.IsDirty(aUpdatableTail)) {
      m.updatableHead = &aUpdatableHead;
      m.updatableTail = &aUpdatableTail;
    }
    if (m.resourceHead || m.updatableHead) {
      m.waiting = true;
      while (m.waiting) {
        if (!m.cond.Wait()) {
          m.waiting = false;
          VRB_LOG("ConditionVarible Wait failed in ContextSynchronizer");
        }
      }
      m.resourceHead = nullptr;
      m.resourceTail = nullptr;
      m.updatableHead = nullptr;
      m.updatableTail = nullptr;
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
    VRB_LOG("ContextSynchronizer::Signal() failed. RenderContext not defined.");
    aIsActive = false;
    return;
  }
  if (!m.context->IsOnRenderThread()) {
    VRB_LOG("ContextSynchronizer::Signal() failed. Must be called on main render thread");
    aIsActive = false;
    return;
  }
  MutexAutoLock lock(m.cond);
  aIsActive = m.active;
  if (m.waiting) {
    m.waiting = false;
    if (m.resourceHead && m.resourceTail) {
      m.context->GetResourceGLTail().PrependAndAdoptList(*m.resourceHead, *m.resourceTail);
    }
    if (m.updatableHead && m.updatableTail) {
      m.context->GetUpdatableTail().PrependAndAdoptList(*m.updatableHead, *m.updatableTail);
    }
    {
      MutexAutoLock(m.observerLock);

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

