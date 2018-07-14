/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/SharedEGLContext.h"
#include "vrb/ConcreteClass.h"

#include "vrb/EGLError.h"
#include "vrb/Logger.h"

#include <EGL/egl.h>
#include <string>

namespace vrb {

static const char* kSurfacelessContextExtenionName = "EGL_KHR_surfaceless_context";

struct SharedEGLContext::State {
  EGLDisplay display;
  EGLConfig config;
  EGLSurface surface;
  EGLContext context;
  bool supportSurfacelessContext;
  State()
      : display(EGL_NO_DISPLAY)
      , config(nullptr)
      , surface(EGL_NO_SURFACE)
      , context(EGL_NO_CONTEXT)
      , supportSurfacelessContext(false)
  {}
};

SharedEGLContextPtr
SharedEGLContext::Create() {
  return std::make_shared<ConcreteClass<SharedEGLContext, SharedEGLContext::State> >();
}

bool
SharedEGLContext::Initialize() {
  m.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  EGLContext currentContext = eglGetCurrentContext();
  if (currentContext == EGL_NO_CONTEXT) {
    VRB_ERROR("Failed to initialize shared egl context, no current context: %s", EGLErrorCheck());
    return false;
  }
  EGLint id = 0;
  eglQueryContext(m.display, currentContext, EGL_CONFIG_ID, &id);
  EGLint attr[] = { EGL_CONFIG_ID, id, EGL_NONE };
  EGLint configCount = 0;
  if (eglChooseConfig(m.display, attr, &(m.config), 1, &configCount) == EGL_FALSE) {
    VRB_ERROR("Failed to find config for shared context %s", EGLErrorCheck());
    return false;
  }
  EGLint contextAttr[] = {
      EGL_CONTEXT_CLIENT_VERSION, 3,
      EGL_NONE
  };
  m.context = eglCreateContext(m.display, m.config, currentContext, contextAttr);

  if (m.context == EGL_NO_CONTEXT) {
    VRB_ERROR("Failed to create shared egl context: %s", EGLErrorString());
    return false;
  }

  const std::string extensions = eglQueryString(m.display, EGL_EXTENSIONS);
  if (extensions.find(kSurfacelessContextExtenionName) == std::string::npos) {
    VRB_WARN("EGL surfaceless context not supported. Falling back to pbuffer surface");
    const EGLint attr[] = {
        EGL_WIDTH, 16,
        EGL_HEIGHT, 16,
        EGL_NONE
    };
    m.surface = VRB_EGL_CHECK(eglCreatePbufferSurface(m.display, m.config, attr));
    if (m.surface == EGL_NO_SURFACE) {
      VRB_ERROR("Failed to create fallback pbuffer surface for shared context");
      return false;
    }
  } else {
    VRB_LOG("EGL surfaceless context supported");
    m.supportSurfacelessContext = true;
  }


  return true;
}

bool
SharedEGLContext::MakeCurrent() {
  if (m.context == EGL_NO_CONTEXT) {
    VRB_ERROR("Failed to make shared egl context current: EGL_NO_CONTEXT");
    return false;
  }
  if (eglMakeCurrent(m.display, m.surface, m.surface, m.context) == EGL_FALSE) {
    VRB_ERROR("Failed to make shared egl context current: %s", EGLErrorString());
    return false;
  }
  return true;
}

SharedEGLContext::SharedEGLContext(State& aState) : m(aState) {}
SharedEGLContext::~SharedEGLContext() {
  if (m.display == EGL_NO_DISPLAY) {
    return;
  }
  if (m.surface != EGL_NO_SURFACE) {
    VRB_EGL_CHECK(eglDestroySurface(m.display, m.surface));
  }
  if (m.context != EGL_NO_CONTEXT) {
    VRB_EGL_CHECK(eglDestroyContext(m.display, m.context));
  }
}

} // namespace vrb
