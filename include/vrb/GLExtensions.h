#ifndef VRB_GL_EXTENSIONS_DOT_H
#define VRB_GL_EXTENSIONS_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include "vrb/gl.h"

#if !defined(GL_EXT_multisampled_render_to_texture)
typedef void (GL_APIENTRY* PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (GL_APIENTRY* PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLsizei samples);
#endif

#if !defined(GL_OVR_multiview)
static const int GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR       = 0x9630;
static const int GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR = 0x9632;
static const int GL_MAX_VIEWS_OVR                                      = 0x9631;
typedef void (GL_APIENTRY* PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint baseViewIndex, GLsizei numViews);
#endif

#if !defined(GL_OVR_multiview_multisampled_render_to_texture)
typedef void (GL_APIENTRY* PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVRPROC)(GLenum target, GLenum attachment, GLuint texture, GLint level, GLsizei samples, GLint baseViewIndex, GLsizei numViews);
#endif

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
