/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/ClassLoaderAndroid.h"

#include "vrb/ConcreteClass.h"
#include "vrb/JNIException.h"
#include "vrb/Logger.h"

#include <jni.h>

namespace vrb {

struct ClassLoaderAndroid::State {
  JNIEnv* env;
  jobject classLoader;
  jmethodID findClass;
  State()
      : env(nullptr)
      , classLoader(nullptr)
      , findClass(nullptr)
  {}

  void Init(JNIEnv* aEnv, jobject& aActivity) {
    env = aEnv;
    jclass clazz = env->GetObjectClass(aActivity);
    // Get class loader instance from the activity
    jmethodID getClassLoader = env->GetMethodID(clazz, "getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject cls = env->CallObjectMethod(aActivity, getClassLoader);
    VRB_CHECK_JNI_EXCEPTION(env);
    classLoader = env->NewGlobalRef(cls);
    // Cache the classLoader->findClass method
    findClass = env->GetMethodID(env->GetObjectClass(classLoader), "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    env->DeleteLocalRef(cls);
  }

  void Shutdown() {
    if (classLoader) {
      env->DeleteGlobalRef(classLoader);
      classLoader = nullptr;
    }
  }
};

ClassLoaderAndroidPtr
ClassLoaderAndroid::Create() {
  return std::make_shared<ConcreteClass<ClassLoaderAndroid, ClassLoaderAndroid::State>>();
}

void
ClassLoaderAndroid::Init(JNIEnv* aEnv, jobject& aActivity) {
  m.Init(aEnv, aActivity);
}

void
ClassLoaderAndroid::Shutdown() {
  m.Shutdown();
}

jclass
ClassLoaderAndroid::FindClass(const std::string& aClassName) const {
  jstring name = m.env->NewStringUTF(aClassName.c_str());
  jclass result = (jclass)(m.env->CallObjectMethod(m.classLoader, m.findClass, name));
  VRB_CHECK_JNI_EXCEPTION(m.env);
  m.env->DeleteLocalRef(name);
  return result;
}

ClassLoaderAndroid::ClassLoaderAndroid(State& aState) : m(aState) {}
ClassLoaderAndroid::~ClassLoaderAndroid() {
  m.Shutdown();
}

}
