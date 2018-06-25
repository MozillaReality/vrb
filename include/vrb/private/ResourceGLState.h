/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_RESOURCE_GL_STATE_DOT_H
#define VRB_RESOURCE_GL_STATE_DOT_H

#include "vrb/ResourceGL.h"

namespace vrb {

struct ResourceGL::State {
  ResourceGL* prevResource;
  ResourceGL* nextResource;

  State() : prevResource(nullptr), nextResource(nullptr) {}
  ~State() {
    if (prevResource) { prevResource->m.nextResource = nextResource; }
    if (nextResource) { nextResource->m.prevResource = prevResource; }
  }
  void CallAllInitializeGL(RenderContext& aContext) {
    ResourceGL* current = nextResource;
    while(current) {
      ResourceGL* tmp = current;
      current = current->m.nextResource;
      tmp->InitializeGL(aContext);
    }
  }
  void CallAllShutdownGL(RenderContext& aContext) {
    ResourceGL* current = nextResource;
    while(current) {
      ResourceGL* tmp = current;
      current = current->m.nextResource;
      tmp->ShutdownGL(aContext);
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
};

class ResourceGLHead : public vrb::ResourceGL {
public:
  ResourceGLHead(): vrb::ResourceGL(m) {}
  ~ResourceGLHead() {}

  // vrb::ResourcGL interface
  void InitializeGL(RenderContext& aContext) override {
    m.CallAllInitializeGL(aContext);
  }
  void ShutdownGL(RenderContext& aContext) override  {
    m.CallAllShutdownGL(aContext);
  }

  // ResourceGLHead interface
  void BindTail(ResourceGLHead& aTail) {
    m.nextResource = &aTail;
    aTail.m.prevResource = this;
  }

  bool Update(RenderContext& aContext) {
    if (!m.nextResource) {
      return false;
    }
    m.CallAllInitializeGL(aContext);
    return true;
  }

  bool IsDirty(ResourceGLHead& aTail) {
    return m.nextResource != &aTail;
  }
protected:
  vrb::ResourceGL::State m;
};

class ResourceGLTail : public ResourceGLHead {
public:
  void InitializeGL(RenderContext&) override {} // noop
  void ShutdownGL(RenderContext&) override {} // noop

  void Prepend(vrb::ResourceGL* aResource)  {
    m.Prepend(aResource);
  }

  void PrependAndAdoptList(ResourceGLHead& aHead, ResourceGLHead& aTail)  {
    m.PrependAndAdoptList(*this, aHead, aTail);
  }
};

} // namespace vrb

#endif // VRB_RESOURCE_GL_STATE_DOT_H
