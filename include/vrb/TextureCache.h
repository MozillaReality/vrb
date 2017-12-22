#ifndef VRB_TEXTURE_CACHE_DOT_H
#define VRB_TEXTURE_CACHE_DOT_H

#include "vrb/Forward.h"

#include <string>

namespace vrb {

class TextureCache {
public:
  static TextureCachePtr Create();

  void Init();
  void Shutdown();

  TexturePtr LoadTexture(const std::string& aFileName);

protected:
  TextureCache();
  ~TextureCache();

  struct State;
  State *m;

private:
  TextureCache(const TextureCache&) = delete;
  TextureCache& operator=(const TextureCache&) = delete;
};

}

#endif // VRB_TEXTURE_CACHE_DOT_H
