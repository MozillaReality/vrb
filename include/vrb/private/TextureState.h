#ifndef VRB_TEXTURE_STATE_DOT_H
#define VRB_TEXTURE_STATE_DOT_H

#include "vrb/Texture.h"

#include <GLES2/gl2.h>
#include <string>

namespace vrb {

struct Texture::State {
  std::string name;
  GLenum target;
  GLuint texture;

  State() : target(GL_TEXTURE_2D), texture(0) {}
};

} // namespace vrb

#endif //  VRB_TEXTURE_STATE_DOT_H
