#ifndef VRB_TEXTURE_CACHE_DOT_H
#define VRB_TEXTURE_CACHE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include <string>

namespace vrb {

class TextureCache {
public:
  static TextureCachePtr Create(ContextWeak& aContext);
  void Init();
  void Shutdown();
  TexturePtr LoadTexture(const std::string& aFileName);
protected:
  struct State;
  TextureCache(State& aState, ContextWeak& aContext);
  ~TextureCache();
private:
  State &m;
  TextureCache() = delete;
  VRB_NO_DEFAULTS(TextureCache)
};

}

#endif // VRB_TEXTURE_CACHE_DOT_H
