#ifndef VRB_JNI_EXCEPTION_DOT_H
#define VRB_JNI_EXCEPTION_DOT_H

#include "vrb/Logger.h"

#include "jni.h"

namespace vrb {

inline void
CheckJNIException(JNIEnv* aEnv, const char* aName, const char* aFile, const int aLine) {
  if (!aEnv) {
    return;
  }
  if (aEnv->ExceptionCheck() == JNI_TRUE) {
    aEnv->ExceptionDescribe();
    aEnv->ExceptionClear();
    VRB_ERROR("Java exception encountered when calling %s in %s:%d", aName, aFile, aLine);
  }
}

} // namespace vrb

#define VRB_CHECK_JNI_EXCEPTION(env) \
  vrb::CheckJNIException(env, __FUNCTION__, __FILE__, __LINE__)

#endif // VRB_JNI_EXCEPTION_DOT_H
