#ifndef VRB_CONTEXT_DOT_H
#define VRB_CONTEXT_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#if defined(ANDROID)
#  include <jni.h>
#endif // defined(ANDROID)

namespace vrb {

class Context {
public:
  static ContextPtr Create();

#if defined(ANDROID)
  void InitializeJava(JNIEnv* aEnv, jobject& aAssetManager);
#endif // defined(ANDROID)
  void InitializeGL();
  void Update();
  void Shutdown();

  void AddResourceGL(ResourceGL* aResource);
  FileReaderPtr GetFileReader();
  TextureCachePtr GetTextureCache();

protected:
  struct State;
  Context(State& aState);
  ~Context();

private:
  State& m;
  Context() = delete;
  VRB_NO_DEFAULTS(Context)
};

}

#endif // VRB_CONTEXT_DOT_H
