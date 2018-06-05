/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_CLASS_LOADER_ANDROID_DOT_H
#define VRB_CLASS_LOADER_ANDROID_DOT_H

#include "vrb/FileReader.h"
#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include <jni.h>
#include <memory>

namespace vrb {

class ClassLoaderAndroid {
public:
  static ClassLoaderAndroidPtr Create(ContextWeak& aContext);
  static ClassLoaderAndroidPtr Create();
  void Init(JNIEnv* aEnv, jobject& aActivity);
  void Shutdown();
  jclass FindClass(const std::string& aClassName) const;
protected:
  struct State;
  ClassLoaderAndroid(State& aState);
  ~ClassLoaderAndroid();
private:
  State& m;
  ClassLoaderAndroid() = delete;
  VRB_NO_DEFAULTS(ClassLoaderAndroid)
};

} // namespace vrb


#endif // VRB_CLASS_LOADER_ANDROID_DOT_H
