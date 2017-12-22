#include "vrb/TextureCache.h"

#include "vrb/Base.h"
#include "vrb/Texture.h"

namespace vrb {

struct TextureCache::State {

};

TextureCachePtr
TextureCache::Create() {
  return std::make_shared<Alloc<TextureCache, TextureCache::State> >();
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

TextureCache::TextureCache() : m(nullptr) {}
TextureCache::~TextureCache() {}

} // namespace vrb
