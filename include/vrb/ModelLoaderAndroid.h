/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_MODEL_LOADER_ANDROID_DOT_H
#define VRB_MODEL_LOADER_ANDROID_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/ResourceGL.h"
#include <functional>
#include <string>
#include <jni.h>

namespace vrb {

typedef std::function<void(GroupPtr&)> LoadFinishedCallback;

class ModelLoaderAndroid {
public:
  static ModelLoaderAndroidPtr Create(RenderContextPtr& aContext);
  void InitializeJava(JNIEnv* eEnv, jobject aActivity, jobject aAssets);
  void ShutdownJava();
  void InitializeGL();
  void ShutdownGL();
  void LoadModel(const std::string& aModelName, GroupPtr aTargetNode);
  void LoadModel(const std::string& aModelName, GroupPtr aTargetNode, LoadFinishedCallback& aCallback);
protected:
  struct State;
  ModelLoaderAndroid(State& aState, RenderContextPtr& aContext);
  ~ModelLoaderAndroid();
private:
  State& m;
  static void* Run(void* data);
  ModelLoaderAndroid() = delete;
  VRB_NO_DEFAULTS(ModelLoaderAndroid);
};

} // namespace vrb

#endif // VRB_MODEL_LOADER_ANDROID_DOT_H
