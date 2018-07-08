/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_RESOURCE_GL_STATE_DOT_H
#define VRB_RESOURCE_GL_STATE_DOT_H

#include "vrb/ResourceGL.h"
#include "vrb/Logger.h"

namespace vrb {

struct ResourceGL::State {
  ResourceGL* prevResource;
  ResourceGL* nextResource;

  State() : prevResource(nullptr), nextResource(nullptr) {}
  ~State() {
    if (prevResource) { prevResource->m.nextResource = nextResource; }
    if (nextResource) { nextResource->m.prevResource = prevResource; }
  }
  void CallAllInitializeGL() {
    ResourceGL* current = nextResource;
    while(current) {
      ResourceGL* tmp = current;
      current = current->m.nextResource;
      tmp->InitializeGL();
    }
  }
  void CallAllShutdownGL() {
    ResourceGL* current = nextResource;
    while(current) {
      ResourceGL* tmp = current;
      current = current->m.nextResource;
      tmp->ShutdownGL();
    }
  }

  void Prepend(ResourceGL* aResource) {
    aResource->m.nextResource = prevResource->m.nextResource;
    aResource->m.prevResource = prevResource;
    prevResource->m.nextResource = aResource;
    prevResource = aResource;
  }

  void PrependAndAdoptList(ResourceGL& self, ResourceGL& aHead, ResourceGL& aTail) {
    if (!prevResource) {
      return;
    }

    if (!aHead.m.nextResource) {
      return;
    }

    if (!aTail.m.prevResource) {
      return;
    }

    prevResource->m.nextResource = aHead.m.nextResource;
    aHead.m.nextResource->m.prevResource = prevResource;
    prevResource = aTail.m.prevResource;
    aTail.m.prevResource->m.nextResource = &self;

    aTail.m.prevResource = &aHead;
    aHead.m.nextResource = &aTail;
  }

  void RemoveFromCurrentList() {
    if (!prevResource || !nextResource) {
      // can't remove head or tail
      VRB_LOG("List %s can not be removed", (prevResource == nullptr ? "head" : "tail"));
      return;
    }
    prevResource->m.nextResource = nextResource;
    nextResource->m.prevResource = prevResource;
    nextResource = prevResource = nullptr;
  }

  void GetOffRenderThreadResources(ResourceGLList& aTail);
};

class ResourceGLTail : public ResourceGL {
public:
  ResourceGLTail() : ResourceGL(m) {}
  void InitializeGL() override {} // noop
  void ShutdownGL() override {} // noop

  void SetHead(ResourceGL* aHead) {
    m.prevResource = aHead;
  }
  void Prepend(vrb::ResourceGL* aResource)  {
    m.Prepend(aResource);
  }

  void PrependAndAdoptList(ResourceGL& aHead, ResourceGL& aTail) {
    m.PrependAndAdoptList(*this, aHead, aTail);
  }

protected:
  ResourceGL::State m;
};

class ResourceGLList : public ResourceGL {
public:
  ResourceGLList() : ResourceGL(m) {
    m.nextResource = &mTail;
    mTail.SetHead(this);
  }
  ~ResourceGLList() {}

  void GetOffRenderThreadResources(ResourceGLList& aList) {
    m.GetOffRenderThreadResources(aList);
  }

  bool Update() {
    if (!m.nextResource) {
      return false;
    }
    m.CallAllInitializeGL();
    return true;
  }

  bool IsDirty() {
    return m.nextResource != &mTail;
  }

  void Append(vrb::ResourceGL* aResource)  {
    mTail.Prepend(aResource);
  }

  void AppendAndAdoptList(ResourceGLList& aList)  {
    mTail.PrependAndAdoptList(aList, aList.mTail);
  }

  // vrb::ResourcGL interface
  void InitializeGL() override {
    m.CallAllInitializeGL();
  }
  void ShutdownGL() override  {
    m.CallAllShutdownGL();
  }
protected:
  ResourceGLTail mTail;
  ResourceGL::State m;
};

inline void
ResourceGL::State::GetOffRenderThreadResources(ResourceGLList& aList) {
  ResourceGL* current = nextResource;
  while (current->m.nextResource) {
    ResourceGL* resource = current;
    current = current->m.nextResource;
    if (resource->SupportOffRenderThreadInitialization()) {
      resource->m.RemoveFromCurrentList();
      aList.Append(resource);
    }
  }
}

} // namespace vrb

#endif // VRB_RESOURCE_GL_STATE_DOT_H
