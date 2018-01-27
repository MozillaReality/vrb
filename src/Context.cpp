#include "vrb/Context.h"
#include "vrb/private/ResourceGLState.h"

#include "vrb/ConcreteClass.h"
#if defined(ANDROID)
#include "vrb/FileReaderAndroid.h"
#endif // defined(ANDROID)
#include "vrb/Logger.h"
#include "vrb/ResourceGL.h"
#include "vrb/TextureCache.h"
#include <EGL/egl.h>

namespace {

class ResourceAnchor : public vrb::ResourceGL {
public:
  ResourceAnchor();
  ~ResourceAnchor();

  // vrb::ResourcGL interface
  void InitializeGL(vrb::Context& aContext) override;
  void ShutdownGL(vrb::Context& aContext) override;

  // ResourceAnchor // interface
  void BindTail(ResourceAnchor& aTail);
  bool Update(vrb::Context& aContex);
  void Prepend(vrb::ResourceGL* aResource);
  void PrependAndAdoptList(ResourceAnchor& aHead, ResourceAnchor& aTail);
protected:
  vrb::ResourceGL::State m;
};

class ResourceAnchorTail : public ResourceAnchor {
public:
  void InitializeGL(vrb::Context&) override {} // noop
  void ShutdownGL(vrb::Context&) override {} // noop
};

ResourceAnchor::ResourceAnchor() : vrb::ResourceGL(m) {}
ResourceAnchor::~ResourceAnchor() {}

void
ResourceAnchor::BindTail(ResourceAnchor& aTail) {
  m.nextResource = &aTail;
  aTail.m.prevResource = this;
}

void
ResourceAnchor::InitializeGL(vrb::Context& aContext) {
  m.CallAllInitializeGL(aContext);
}

void
ResourceAnchor::ShutdownGL(vrb::Context& aContext) {
  m.CallAllShutdownGL(aContext);
}

bool
ResourceAnchor::Update(vrb::Context& aContext) {
  if (!m.nextResource) {
    return false;
  }
  m.CallAllInitializeGL(aContext);
  return true;
}

void
ResourceAnchor::Prepend(vrb::ResourceGL* aResource) {
  m.Prepend(aResource);
}

void
ResourceAnchor::PrependAndAdoptList(ResourceAnchor& aHead, ResourceAnchor& aTail) {
  m.PrependAndAdoptList(*this, aHead, aTail);
}

}

namespace vrb {

struct Context::State {
  std::weak_ptr<Context> self;
  EGLContext eglContext;
  TextureCachePtr textureCache;
#if defined(ANDROID)
  FileReaderAndroidPtr fileReader;
#endif // defined(ANDROID)
  ResourceAnchor addedResourcesHead;
  ResourceAnchorTail addedResourcesTail;
  ResourceAnchor resourcesHead;
  ResourceAnchorTail resourcesTail;
  State();
};

Context::State::State() : eglContext(EGL_NO_CONTEXT) {
  addedResourcesHead.BindTail(addedResourcesTail);
  resourcesHead.BindTail(resourcesTail);
}

ContextPtr
Context::Create() {
  ContextPtr result = std::make_shared<ConcreteClass<Context, Context::State> >();
  result->m.self = result;
  result->m.textureCache = TextureCache::Create(result->m.self);
#if defined(ANDROID)
  result->m.fileReader = FileReaderAndroid::Create(result->m.self);
#endif // defined(ANDROID)

  return result;
}

#if defined(ANDROID)
void
Context::InitializeJava(JNIEnv* aEnv, jobject& aAssetManager) {
  if (m.fileReader) { m.fileReader->Init(aEnv, aAssetManager); }
}
#endif // defined(ANDROID)

void
Context::InitializeGL() {
  EGLContext current = eglGetCurrentContext();
  if (current == EGL_NO_CONTEXT) {
    VRB_LOG("Unable to initialize VRB context: EGLContext is not valid.");
    m.eglContext = current;
    return;
  }
  if (current == m.eglContext) {
    VRB_LOG("EGLContext c:%p == %p",(void*)current,(void*)m.eglContext);
  } else {
    VRB_LOG("*** EGLContext NOT EQUAL %p != %p",(void*)current,(void*)m.eglContext);
  }
  m.eglContext = current;

  m.resourcesHead.InitializeGL(*this);
}


void
Context::Update() {
  if (!m.addedResourcesHead.Update(*this)) {
    return;
  }

  m.resourcesTail.PrependAndAdoptList(m.addedResourcesHead, m.addedResourcesTail);
}

void
Context::Shutdown() {
  m.resourcesHead.ShutdownGL(*this);
#if defined(ANDROID)
  if (m.fileReader) { m.fileReader->Shutdown(); }
#endif // defined(ANDROID)
  m.eglContext = EGL_NO_CONTEXT;
}

FileReaderPtr
Context::GetFileReader() {
#if defined(ANDROID)
  return m.fileReader;
#else
#  error "Platform not supported"
#endif // defined(ANDROID)
}

void
Context::AddResourceGL(ResourceGL* aResource) {
  m.addedResourcesTail.Prepend(aResource);
}

TextureCachePtr
Context::GetTextureCache() {
  return m.textureCache;
}

Context::Context(State& aState) : m(aState) {}
Context::~Context() {}

} // namespace vrb
