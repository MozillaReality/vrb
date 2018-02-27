/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/FBO.h"
#include "vrb/ConcreteClass.h"

#include "vrb/Context.h"
#include "vrb/GLError.h"
#include "vrb/GLExtensions.h"
#include "vrb/Logger.h"

namespace vrb {

struct FBO::State {
  ContextWeak context;
  bool valid;
  GLuint depth;
  GLuint fbo;
  Attributes attributes;

  State() : depth(0), fbo(0), valid(false) {}
  void Clear() {
    if (depth) {
      if (attributes.multiview) {
        glDeleteTextures(1, &depth);
      } else {
        glDeleteRenderbuffers(1, &depth);
      }
      depth = 0;
    }
    if (fbo) {
      glDeleteFramebuffers(1, &fbo);
      fbo = 0;
    }
    valid = false;
  }

  void UpdateAttributes(const FBO::Attributes& aAttributes) {
    attributes = aAttributes;
    ContextPtr ctx = context.lock();
    GLExtensionsPtr ext = ctx->GetGLExtensions();
    if (attributes.multiview) {
      if (!ext->IsExtensionSupported(GLExtensions::Ext::OVR_multiview)) {
        attributes.multiview = false;
        VRB_LOG("Multiview not supported");
      }
      if (attributes.samples > 1 &&
          !ext->IsExtensionSupported(GLExtensions::Ext::EXT_multisampled_render_to_texture)) {
        attributes.samples = 1;
        VRB_LOG("Multiview multisampled not supported");
      }
    }
    else if (attributes.samples > 1 &&
             !ext->IsExtensionSupported(GLExtensions::Ext::EXT_multisampled_render_to_texture)) {
      attributes.samples = 1;
      VRB_LOG("Multiview multisampled not supported");
    }
  }

  void InitializeMultiview(const GLuint aHandle, int32_t aWidth, int32_t aHeight) {
    ContextPtr ctx = context.lock();
    const GLExtensions::Functions& ext = ctx->GetGLExtensions()->GetFunctions();

    if (attributes.depth) {
      // Create the depth buffer texture.
      VRB_CHECK(glGenTextures(1, &depth));
      VRB_CHECK(glBindTexture(GL_TEXTURE_2D_ARRAY, depth));
      VRB_CHECK(glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT24, aWidth, aHeight, 2));
      VRB_CHECK(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
    }

    VRB_CHECK(glGenFramebuffers(1, &fbo));
    VRB_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo));
    if (attributes.samples > 1) {
      VRB_CHECK(ext.glFramebufferTextureMultisampleMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                                                depth, 0, attributes.samples, 0 , 2));
      VRB_CHECK(ext.glFramebufferTextureMultisampleMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                                                aHandle, 0, attributes.samples, 0, 2));
    } else {
      VRB_CHECK(ext.glFramebufferTextureMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                                     depth, 0, 0 , 2));
      VRB_CHECK(ext.glFramebufferTextureMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                                     aHandle, 0, 0, 2));
    }
  }

  void Initialize(const GLuint aHandle, int32_t aWidth, int32_t aHeight) {
    ContextPtr ctx = context.lock();
    const GLExtensions::Functions& ext = ctx->GetGLExtensions()->GetFunctions();

    VRB_CHECK(glGenFramebuffers(1, &fbo));
    VRB_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

    if (attributes.depth) {
      VRB_CHECK(glGenRenderbuffers(1, &depth));
      VRB_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, depth));
      if (attributes.samples > 1) {
        VRB_CHECK(ext.glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, attributes.samples, GL_DEPTH_COMPONENT24, aWidth, aHeight));
      } else {
        VRB_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, aWidth, aHeight));
      }
      VRB_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth));
    }

    if (attributes.samples > 1) {
      VRB_CHECK(ext.glFramebufferTexture2DMultisampleEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, aHandle, 0, attributes.samples));
    } else {
      VRB_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, aHandle, 0));
    }
  }
};

FBO::Attributes::Attributes()
  : depth(true)
  , multiview(false)
  , samples(0) {}

FBO::Attributes::Attributes(bool aDepth, bool aMultiview, int aSamples)
  : depth(aDepth)
  , multiview(aMultiview)
  , samples(aSamples) {}

FBOPtr
FBO::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<FBO, FBO::State> >(aContext);
}

bool
FBO::IsValid() const {
  return m.valid;
}

void
FBO::SetTextureHandle(const GLuint aHandle,
                      const int32_t aWidth,
                      const int32_t aHeight,
                      const FBO::Attributes& aAttributtes) {
  m.Clear();
  if (aHandle) {
    m.UpdateAttributes(aAttributtes);

    if (m.attributes.multiview) {
      m.InitializeMultiview(aHandle, aWidth, aHeight);
    } else {
      m.Initialize(aHandle, aWidth, aHeight);
    }

    if (GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
      m.valid = true;
    } else {
      VRB_LOG("Failed to create valid frame buffer object");
      m.Clear();
    }
    Unbind();
  }
}

void
FBO::Bind() {
  if (!m.valid) { return; }
  VRB_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m.fbo));
}

void
FBO::Unbind() {
  VRB_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

const FBO::Attributes&
FBO::GetAttributes() const {
  return m.attributes;
}

FBO::FBO(State& aState, ContextWeak& aContext) : m(aState) { m.context = aContext; }
FBO::~FBO() { m.Clear(); }

} // namespace vrb
