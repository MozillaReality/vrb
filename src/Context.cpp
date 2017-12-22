#include "vrb/Context.h"

#include "vrb/Base.h"
#include "vrb/Logger.h"
#include "vrb/TextureCache.h"

#include <EGL/egl.h>

namespace vrb {

struct Context::State {
  EGLContext eglContext;
  TextureCachePtr textureCache;
  State();
};

Context::State::State()
    : eglContext(EGL_NO_CONTEXT)
{
  textureCache = TextureCache::Create();
};

ContextPtr
Context::Create() {
  return std::make_shared<Alloc<Context, Context::State> >();
}

void
Context::Init() {
  EGLContext current = eglGetCurrentContext();
  if (current == EGL_NO_CONTEXT) {
    VRB_LOG("Unable to initialize VRB context: EGLContext is not valid.");
    m->eglContext = current;
    return;
  }
  if (current == m->eglContext) {
    VRB_LOG("EGLContext c:%p == %p",(void*)current,(void*)m->eglContext);
  } else {
    VRB_LOG("*** EGLContext NOT EQUAL %p != %p",(void*)current,(void*)m->eglContext);
  }
  m->eglContext = current;
  if (m->textureCache) { m->textureCache->Init(); }
}

void
Context::Shutdown() {
  if (m->textureCache) { m->textureCache->Shutdown(); }
}

TextureCachePtr&
Context::GetTextureCache() {
  return m->textureCache;
}

Context::Context() : m(nullptr) {}

Context::~Context() {}

} // namespace vrb
