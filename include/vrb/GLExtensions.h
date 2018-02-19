#ifndef VRB_GL_EXTENSIONS_DOT_H
#define VRB_GL_EXTENSIONS_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include "vrb/gl.h"

namespace vrb {

class GLExtensions {
public:
  // Known GL extensions that can be queried by IsExtensionSupported.
  enum class Ext {
    EXT_multisampled_render_to_texture,
    OVR_multiview,
    OVR_multiview2,
    OVR_multiview_multisampled_render_to_texture
  };

  // GL extension function pointers
  struct Functions {
    PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glRenderbufferStorageMultisampleEXT;
    PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC glFramebufferTexture2DMultisampleEXT;
    PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC glFramebufferTextureMultiviewOVR;
    PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVRPROC glFramebufferTextureMultisampleMultiviewOVR;
  };

  static GLExtensionsPtr Create(ContextWeak& aContext);
  void Initialize();
  bool IsExtensionSupported(GLExtensions::Ext aExtension) const;
  const GLExtensions::Functions & GetFunctions() const;
protected:
  struct State;
  GLExtensions(State& aState, ContextWeak& aContext);
  ~GLExtensions();
private:
  State& m;
  GLExtensions() = delete;
  VRB_NO_DEFAULTS(GLExtensions)
};

}
#endif // VRB_GL_EXTENSIONS_DOT_H
