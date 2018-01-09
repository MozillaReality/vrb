#include "vrb/FileReaderAndroid.h"

#include "vrb/ConcreteClass.h"
#include "vrb/Logger.h"

#include <jni.h>
#include <vector>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#define JNI_METHOD(return_type, method_name) \
  JNIEXPORT return_type JNICALL              \
    Java_org_mozilla_vrb_ImageLoader_##method_name


namespace {

inline jlong jptr(vrb::FileReaderAndroid* ptr) { return reinterpret_cast<intptr_t>(ptr); }
inline vrb::FileReaderAndroid* ptr(jlong jptr) { return reinterpret_cast<vrb::FileReaderAndroid*>(jptr); }

}

namespace vrb {

struct FileReaderAndroid::State {
  int trackingHandleCount;
  JNIEnv* env;
  jobject jassetManager;
  AAssetManager* am;
  jclass imageLoaderClass;
  jmethodID loadFromAssets;
  int imageTargetHandle;
  FileHandlerPtr imageTarget;
  State()
      : trackingHandleCount(0)
      , env(nullptr)
      , jassetManager(0)
      , am(nullptr)
      , imageLoaderClass(0)
      , loadFromAssets(0)
      , imageTargetHandle(0)
  {}

  int nextHandle() {
    trackingHandleCount++;
    return trackingHandleCount;
  }

private:
  State(const State&) = delete;
  State& operator=(const State&) = delete;
};

FileReaderAndroidPtr
FileReaderAndroid::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<FileReaderAndroid, FileReaderAndroid::State> >(aContext);
}

void
FileReaderAndroid::ReadRawFile(const std::string& aFileName, FileHandlerPtr aHandler) {
  const int handle = m.nextHandle();
  aHandler->BindFileHandle(aFileName, handle);
  if (!m.am) {
    aHandler->LoadFailed(handle, "Unable to load file: No Android AssetManager.");
    return;
  }

  AAsset* asset = AAssetManager_open(m.am, aFileName.c_str(), AASSET_MODE_STREAMING);
  if (!asset) {
    aHandler->LoadFailed(handle, "Unable to find file");
    return;
  }

  const int bufferSize = 1024;
  char buffer[bufferSize];
  int read = 0;
  while ((read = AAsset_read(asset, buffer, bufferSize)) > 0) {
    aHandler->ProcessRawFileChunk(handle, buffer, read);
  }
  if (read == 0) {
    aHandler->FinishRawFile(handle);
  } else {
    aHandler->LoadFailed(handle, "Error while reading file");
  }

  AAsset_close(asset);
}

void
FileReaderAndroid::ReadImageFile(const std::string& aFileName, FileHandlerPtr aHandler) {
  if (!aHandler) {
    return;
  }
  m.imageTarget = aHandler;
  m.imageTargetHandle = m.nextHandle();
  m.imageTarget->BindFileHandle(aFileName, m.imageTargetHandle);
  if (!m.loadFromAssets || m.am) {
    m.imageTarget->LoadFailed(m.imageTargetHandle, "FileReaderAndroid is not initialized.");
    return;
  }

  m.env->CallStaticVoidMethod(m.imageLoaderClass, m.loadFromAssets, m.jassetManager, m.env->NewStringUTF(aFileName.c_str()), m.imageTargetHandle);
}

void
FileReaderAndroid::Init(JNIEnv* aEnv, jobject &aAssetManager) {
  m.env = aEnv;
  if (!m.env) {
    return;
  }
  m.jassetManager = m.env->NewGlobalRef(aAssetManager);
  m.am = AAssetManager_fromJava(m.env, m.jassetManager);
  jclass localImageLoaderClass = m.env->FindClass("org/mozilla/vrb/ImageLoader");
  m.imageLoaderClass = (jclass)m.env->NewGlobalRef(localImageLoaderClass);
  m.loadFromAssets = m.env->GetStaticMethodID(m.imageLoaderClass, "loadFromAssets", "(Landroid/content/res/AssetManager;Ljava/lang/String;JI)V");
}

void
FileReaderAndroid::Shutdown() {
  if (m.env) {
    m.env->DeleteGlobalRef(m.jassetManager);
    m.env->DeleteGlobalRef(m.imageLoaderClass);
    m.am = nullptr;
    m.env = nullptr;
    m.loadFromAssets = 0;
  }
}

void
FileReaderAndroid::ProcessImageFile(const int aFileHandle, std::unique_ptr<uint8_t[]> &aImage, const int aWidth, const int aHeight) {
  if (m.imageTargetHandle != aFileHandle) {
    return;
  }

  m.imageTargetHandle = 0;
  if (!m.imageTarget) {
    return;
  }

  m.imageTarget->ProcessImageFile(aFileHandle, aImage, aWidth, aHeight);
  m.imageTarget = nullptr;
}


void
FileReaderAndroid::ImageFileLoadFailed(const int aFileHandle, const std::string& aReason) {
  if (!m.imageTarget || (m.imageTargetHandle != aFileHandle)) {
    return;
  }

  m.imageTarget->LoadFailed(aFileHandle, aReason);
  m.imageTargetHandle = 0;
  m.imageTarget = nullptr;
}

FileReaderAndroid::FileReaderAndroid(State& aState, ContextWeak& aContext) : m(aState) {}
FileReaderAndroid::~FileReaderAndroid() {}

} // namespace vrb

JNI_METHOD(void, ProcessImage)
(JNIEnv* env, jclass, jlong aFileReaderAndroid, int aFileTrackingHandle, jintArray aPixels, int width, int height) {
  vrb::FileReaderAndroid* reader = ptr(aFileReaderAndroid);

  if (!reader) {
    VRB_LOG("Error: nullptr FileReaderAndroid in ProcessTexture");
    return;
  }

  jsize arraySize = env->GetArrayLength(aPixels);
  if ((width * height) > arraySize) {

  }

  const int imageSize = width * height * 4;
  std::unique_ptr<uint8_t[]> image = std::make_unique<uint8_t[]>(imageSize);

  jint* array = env->GetIntArrayElements(aPixels, 0);
  memcpy(image.get(), array, imageSize);
  env->ReleaseIntArrayElements(aPixels, array, 0);

  reader->ProcessImageFile(aFileTrackingHandle, image, width, height);
}

JNI_METHOD(void, ImageLoadFailed)
(JNIEnv* env, jclass, jlong aFileReaderAndroid, int aFileTrackingHandle, jstring aReason) {
  vrb::FileReaderAndroid* reader = ptr(aFileReaderAndroid);

  if (!reader) {
    VRB_LOG("Error: nullptr FileReaderAndroid in ImageLoadFailed");
    return;
  }

  const char *nativeString = env->GetStringUTFChars(aReason, 0);
  std::string reason = nativeString;
  env->ReleaseStringUTFChars(aReason, nativeString);

  reader->ImageFileLoadFailed(aFileTrackingHandle, reason);
}
