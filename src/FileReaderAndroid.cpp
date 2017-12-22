#include "vrb/FileReaderAndroid.h"

#include "vrb/Base.h"
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
  State()
      : trackingHandleCount(0)
      , env(nullptr)
      , jassetManager(0)
      , am(nullptr)
      , imageLoaderClass(0)
      , loadFromAssets(0)
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
FileReaderAndroid::Create() {
  return std::make_shared<Alloc<FileReaderAndroid, FileReaderAndroid::State> >();
}

void
FileReaderAndroid::ReadRawFile(const std::string& aFileName, FileHandlerPtr aHandler) {
  const int handle = m->nextHandle();
  aHandler->Bind(aFileName, handle);
  if (!m->am) {
    aHandler->LoadFailed(handle, "Unable to load file: No Android AssetManager.");
    return;
  }

  AAsset* asset = AAssetManager_open(m->am, aFileName.c_str(), AASSET_MODE_STREAMING);
  if (!asset) {
    aHandler->LoadFailed(handle, "Unable to find file");
    return;
  }

  const int bufferSize = 1024;
  char buffer[bufferSize];
  int read = 0;
  while ((read = AAsset_read(asset, buffer, bufferSize)) > 0) {
    aHandler->ProcessBuffer(handle, buffer, read);
  }
  if (read == 0) {
    aHandler->Finish(handle);
  } else {
    aHandler->LoadFailed(handle, "Error while reading file");
  }

  AAsset_close(asset);
}

void
FileReaderAndroid::ReadImageFile(const std::string& aFileName, FileHandlerPtr aHandler) {
  const int handle = m->nextHandle();
  if (!m->loadFromAssets || m->am) {
    return;
  }
  m->env->CallStaticVoidMethod(m->imageLoaderClass, m->loadFromAssets, m->jassetManager, m->env->NewStringUTF(aFileName.c_str()), handle);
}

void
FileReaderAndroid::SetAssetManager(JNIEnv* aEnv, jobject &aAssetManager) {
  m->env = aEnv;
  if (!m->env) {
    return;
  }
  m->jassetManager = m->env->NewGlobalRef(aAssetManager);
  m->am = AAssetManager_fromJava(m->env, m->jassetManager);
  jclass localImageLoaderClass = m->env->FindClass("mozilla/org/vrb/ImageLoader");
  m->imageLoaderClass = (jclass)m->env->NewGlobalRef(localImageLoaderClass);
  m->loadFromAssets = m->env->GetMethodID(m->imageLoaderClass, "loadFromAssets", "(Landroid/content/res/AssetManager;Ljava/lang/String;JI)V");
}

void
FileReaderAndroid::ClearAssetManager() {
  if (m->env) {
    m->env->DeleteGlobalRef(m->jassetManager);
    m->am = nullptr;
    m->env = nullptr;
    m->env->DeleteGlobalRef(m->imageLoaderClass);
    m->loadFromAssets = 0;
  }
}


FileReaderAndroid::FileReaderAndroid() : m(nullptr) {}
FileReaderAndroid::~FileReaderAndroid() {}

} // namespace vrb

JNI_METHOD(void, ProcessTexture)
(JNIEnv*, jclass, jlong aFileReaderAndroid, int aFileTrackingHandle, jintArray aPixels, int width, int height) {
  vrb::FileReaderAndroid* reader = ptr(aFileReaderAndroid);

  if (!reader) {
    VRB_LOG("Error: void FileReaderAndroid in ProcessTexture");
    return;
  }

}
