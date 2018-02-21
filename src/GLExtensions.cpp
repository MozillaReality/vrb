#include "vrb/GLExtensions.h"
#include "vrb/ConcreteClass.h"

#include "vrb/GLError.h"
#include "vrb/Logger.h"
#include <unordered_set>
#include <string>

#if defined(ANDROID)
#include <EGL/egl.h>
#endif

namespace vrb {

struct GLExtensions::State {
  std::unordered_set<GLExtensions::Ext> supportedExtensions;
  Functions functions;

  State() {
    memset(&functions, 0, sizeof(functions));
  }

  void Initialize() {
    supportedExtensions.clear();

    const char * glStr = (const char *) glGetString( GL_EXTENSIONS );
#define ADD_EXT(n, v) if (strstr(glStr, n)) { supportedExtensions.insert(v); }
    ADD_EXT("GL_EXT_multisampled_render_to_texture", Ext::EXT_multisampled_render_to_texture);
    ADD_EXT("GL_OVR_multiview", Ext::OVR_multiview);
    ADD_EXT("GL_OVR_multiview2", Ext::OVR_multiview2);
    ADD_EXT("OVR_multiview_multisampled_render_to_texture", Ext::OVR_multiview_multisampled_render_to_texture);

#if defined(ANDROID)
#define GET_PROC(n) functions.n = (decltype(functions.n))eglGetProcAddress(#n);
    GET_PROC(glRenderbufferStorageMultisampleEXT);
    GET_PROC(glFramebufferTexture2DMultisampleEXT);
    GET_PROC(glFramebufferTextureMultiviewOVR);
    GET_PROC(glFramebufferTextureMultisampleMultiviewOVR);
#endif
  }
};

GLExtensionsPtr
GLExtensions::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<GLExtensions, GLExtensions::State> >(aContext);
}

void
GLExtensions::Initialize() {
  m.Initialize();
}

bool
GLExtensions::IsExtensionSupported(GLExtensions::Ext aExtension) const {
  return m.supportedExtensions.find(aExtension) != m.supportedExtensions.end();
}

const GLExtensions::Functions &
GLExtensions::GetFunctions() const {
  return m.functions;
}

GLExtensions::GLExtensions(State& aState, ContextWeak& aContext) : m(aState) {}
GLExtensions::~GLExtensions() {}

} // namespace vrb
