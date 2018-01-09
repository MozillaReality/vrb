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
};

} // namespace vrb

#endif // VRB_RESOURCE_GL_STATE_DOT_H
