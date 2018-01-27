#include "vrb/TextureCache.h"

#include "vrb/ConcreteClass.h"
#include "vrb/Context.h"
#include "vrb/DefaultImageData.h"
#include "vrb/FileReader.h"
#include "vrb/Logger.h"
#include "vrb/Texture.h"

#include <unordered_map>

namespace {

class TextureHandler;
typedef std::shared_ptr<TextureHandler> TextureHandlerPtr;

class TextureHandler : public vrb::FileHandler {
public:
  static TextureHandlerPtr Create(const vrb::TexturePtr& aTexture);
  void BindFileHandle(const std::string& aFileName, const int aFileHandle) override;
  void LoadFailed(const int aFileHandle, const std::string& aReason) override;
  void ProcessRawFileChunk(const int aFileHandle, const char* aBuffer, const size_t aSize) override {};
  void FinishRawFile(const int aFileHandle) override {};
  void ProcessImageFile(const int aFileHandle, std::unique_ptr<uint8_t[]>& aImage, const int aWidth, const int aHeight) override;
  TextureHandler() {}
  ~TextureHandler() {}
protected:
  vrb::TexturePtr mTexture;
private:
  VRB_NO_DEFAULTS(TextureHandler)
};

TextureHandlerPtr
TextureHandler::Create(const vrb::TexturePtr& aTexture) {
  TextureHandlerPtr result = std::make_shared<TextureHandler>();
  result->mTexture = aTexture;
  return result;
}

void
TextureHandler::BindFileHandle(const std::string& aFileName, const int aFileHandle) {

}

void
TextureHandler::LoadFailed(const int aFileHandle, const std::string& aReason) {
  VRB_LOG("Failed to load texture: %s", aReason.c_str());
}

void
TextureHandler::ProcessImageFile(const int aFileHandle, std::unique_ptr<uint8_t[]>& aImage, const int aWidth, const int aHeight) {
  mTexture->SetRGBData(aImage, aWidth, aHeight, 4);
}

}

namespace vrb {

struct TextureCache::State {
  ContextWeak context;
  TexturePtr defaultTexture;
  std::unordered_map<std::string, TexturePtr> cache;

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

  std::unordered_map<std::string, TexturePtr>::iterator it = m.cache.find(aFileName);
  if (it != m.cache.end()) {
    return it->second;
  }

  ContextPtr context = m.context.lock();
  if (!context) {
    return m.defaultTexture;
  }

  FileReaderPtr reader = context->GetFileReader();

  if (!reader) {
    return m.defaultTexture;
  }

  result = Texture::Create(m.context);
  result->SetFallbackTexture(m.defaultTexture);
  m.cache[aFileName] = result;
  reader->ReadImageFile(aFileName, TextureHandler::Create(result));

  return result;
}

TextureCache::TextureCache(State& aState, ContextWeak& aContext) : m(aState) {
  m.context = aContext;
  m.defaultTexture = Texture::Create(aContext);
  const size_t kArraySize = kDefaultImageDataSize * sizeof(uint32_t);
  std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(kArraySize);
  memcpy(data.get(), (void*)kDefaultImageData, kArraySize);
  m.defaultTexture->SetRGBData(data, kDefaultImageDataWidth, kDefaultImageDataHeight, 4);
}
TextureCache::~TextureCache() {}

} // namespace vrb
