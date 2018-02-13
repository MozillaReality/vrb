#include "vrb/FBO.h"
#include "vrb/ConcreteClass.h"

#include "vrb/GLError.h"
#include "vrb/Logger.h"


namespace vrb {

struct FBO::State {
  bool valid;
  GLuint depth;
  GLuint fbo;

  State() : depth(0), fbo(0), valid(false) {}
  void Clear() {
    if (depth) {
      glDeleteRenderbuffers(1, &depth);
      depth = 0;
    }
    if (fbo) {
      glDeleteFramebuffers(1, &fbo);
      fbo = 0;
    }
    valid = false;
  }
};

FBOPtr
FBO::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<FBO, FBO::State> >(aContext);
}

bool
FBO::IsValid() const {
  return m.valid;
}

void
FBO::SetTextureHandle(const GLuint aHandle, const int32_t aWidth, const int32_t aHeight) {
  m.Clear();
  if (aHandle) {
    VRB_CHECK(glGenFramebuffers(1, &m.fbo));
    VRB_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m.fbo));

    VRB_CHECK(glGenRenderbuffers(1, &m.depth));
    VRB_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, m.depth));
    VRB_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, aWidth, aHeight));
    VRB_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m.depth));

    VRB_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, aHandle, 0));

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

FBO::FBO(State& aState, ContextWeak& aContext) : m(aState) {}
FBO::~FBO() { m.Clear(); }

} // namespace vrb
