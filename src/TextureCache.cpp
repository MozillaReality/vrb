#include "vrb/TextureCache.h"

#include "vrb/ConcreteClass.h"
#include "vrb/Texture.h"

namespace vrb {

struct TextureCache::State {

};

TextureCachePtr
TextureCache::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<TextureCache, TextureCache::State> >(aContext);
}

void
TextureCache::Init() {

}

void
TextureCache::Shutdown() {

}

TexturePtr
TextureCache::LoadTexture(const std::string& aFileName) {
  TexturePtr result;

  return result;
}

TextureCache::TextureCache(State& aState, ContextWeak& aContext) : m(aState) {}
TextureCache::~TextureCache() {}

} // namespace vrb
