/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/ModelLoaderAndroid.h"
#include "vrb/ConcreteClass.h"

#include "vrb/Group.h"
#include "vrb/ClassLoaderAndroid.h"
#include "vrb/ConditionVariable.h"
#include "vrb/ContextSynchronizer.h"
#include "vrb/CreationContext.h"
#include "vrb/FileReaderAndroid.h"
#include "vrb/Logger.h"
#include "vrb/NodeFactoryObj.h"
#include "vrb/ParserObj.h"
#include "vrb/SharedEGLContext.h"

#include <pthread.h>
#include <vector>
#include <vrb/include/vrb/RenderContext.h>

namespace vrb {

static LoadFinishedCallback sNoop = [](GroupPtr&){};

struct LoadInfo {
  std::string name;
  GroupPtr target;
  LoadFinishedCallback callback;
  LoadInfo(const std::string& aName, GroupPtr& aGroup, LoadFinishedCallback& aCallback)
      : name(aName)
      , target(aGroup)
      , callback(aCallback)
  {}
  LoadInfo(const LoadInfo& aInfo)
      : name(aInfo.name)
      , target(aInfo.target)
      , callback(aInfo.callback)
  {}
  LoadInfo& operator=(const LoadInfo& aInfo) {
    name = aInfo.name;
    target = aInfo.target;
    callback = aInfo.callback;
    return *this;
  }
};

class ModelLoaderAndroidSynchronizerObserver;
typedef std::shared_ptr<ModelLoaderAndroidSynchronizerObserver> ModelLoaderAndroidSynchronizerObserverPtr;

class ModelLoaderAndroidSynchronizerObserver : public ContextSynchronizerObserver {
public:
  static ModelLoaderAndroidSynchronizerObserverPtr Create() {
    return std::make_shared<ModelLoaderAndroidSynchronizerObserver>();
  }
  void Set(GroupPtr& aSource, GroupPtr& aTarget, LoadFinishedCallback& aCallback) {
    mSource = aSource;
    mTarget = aTarget;
    mCallback = aCallback;
  }
  void ContextsSynchronized(RenderContextPtr& aRenderContext) override {
    if (mTarget && mSource) {
      mTarget->TakeChildren(mSource);
      mCallback(mTarget);
      mTarget = mSource = nullptr;
      mCallback = sNoop;
    }
  }
  ModelLoaderAndroidSynchronizerObserver() {}
protected:
  GroupPtr mSource;
  GroupPtr mTarget;
  LoadFinishedCallback mCallback;
private:
  VRB_NO_DEFAULTS(ModelLoaderAndroidSynchronizerObserver)
};

struct ModelLoaderAndroid::State {
  bool running;
  JavaVM* jvm;
  JNIEnv* renderThreadEnv;
  JNIEnv* env;
  jobject activity;
  jobject assets;
  RenderContextWeak render;
  CreationContextPtr context;
  SharedEGLContextPtr eglContext;
  pthread_t child;
  ConditionVariable loadLock;
  bool done;
  bool quitting;
  std::vector<LoadInfo> loadList;
  State()
      : running(false)
      , jvm(nullptr)
      , renderThreadEnv(nullptr)
      , env(nullptr)
      , activity(nullptr)
      , assets(nullptr)
      , done(false)
      , quitting(false)
  {}
  void StartThread() {
    if (running) {
      return;
    }
    if (!renderThreadEnv || !eglContext) {
      return;
    }
    pthread_create(&child, nullptr, &ModelLoaderAndroid::Run, this);
    running = true;
  }

  void StopThread() {
    if (!running) {
      return;
    }
    RenderContextPtr context = render.lock();
    if (context) {
      context->Update();
    }
    VRB_LOG("Waiting for ModelLoaderAndroid load thread to stop.");
    {
      MutexAutoLock lock(loadLock);
      done = true;
      loadLock.Signal();
    }
    bool gotQuit = false;
    while (!gotQuit) {
      if (context) {
        context->Update();
      }
      MutexAutoLock lock(loadLock);
      gotQuit = quitting;
    }
    if (pthread_join(child, nullptr) == 0) {
      VRB_LOG("ModelLoaderAndroid load thread stopped");
    } else {
      VRB_LOG("Error: ModelLoaderAndroid load thread failed to stop");
    }
    running = false;
  }
};

ModelLoaderAndroidPtr
ModelLoaderAndroid::Create(RenderContextPtr& aContext) {

  return std::make_shared<ConcreteClass<ModelLoaderAndroid, ModelLoaderAndroid::State> >(aContext);
}

void
ModelLoaderAndroid::InitializeJava(JNIEnv* aEnv, jobject aActivity, jobject aAssets) {
  if (m.running) {
    ShutdownJava();
  }
  if (aEnv->GetJavaVM(&(m.jvm)) != 0) {
    return;
  }
  m.renderThreadEnv = aEnv;
  m.activity = aEnv->NewGlobalRef(aActivity);
  m.assets = aEnv->NewGlobalRef(aAssets);
  m.StartThread();
}

void
ModelLoaderAndroid::ShutdownJava() {
  m.StopThread();
  if (!m.renderThreadEnv) {
    return;
  }
  if (m.activity) {
    m.renderThreadEnv->DeleteGlobalRef(m.activity);
    m.activity = nullptr;
  }
  if (m.assets) {
    m.renderThreadEnv->DeleteGlobalRef(m.assets);
    m.assets = nullptr;
  }
  m.renderThreadEnv = nullptr;
}

void
ModelLoaderAndroid::InitializeGL() {
  m.eglContext = SharedEGLContext::Create();
  m.eglContext->Initialize();
  m.StartThread();
}

void
ModelLoaderAndroid::ShutdownGL() {
  m.StopThread();
  m.eglContext = nullptr;
}

void
ModelLoaderAndroid::LoadModel(const std::string& aModelName, GroupPtr aTargetNode) {
  LoadModel(aModelName, aTargetNode, sNoop);
}

void
ModelLoaderAndroid::LoadModel(const std::string& aModelName, GroupPtr aTargetNode, LoadFinishedCallback& aCallback) {
  MutexAutoLock(m.loadLock);
  m.loadList.push_back(LoadInfo(aModelName, aTargetNode, aCallback));
  m.loadLock.Signal();
}

/* static */ void*
ModelLoaderAndroid::Run(void* data) {
  ModelLoaderAndroid::State& m = *(ModelLoaderAndroid::State*)data;
  m.context->BindToThread();
  bool attached = false;
  if (m.jvm->AttachCurrentThread(&(m.env), nullptr) == 0) {
    attached = true;
    const bool offRenderThreadContextCurrent = m.eglContext->MakeCurrent();
    if (!offRenderThreadContextCurrent) {
      VRB_LOG("Failed to make shared context current. VRB Nodes will be initialized on render thread");
    }
    ClassLoaderAndroidPtr classLoader = ClassLoaderAndroid::Create();
    classLoader->Init(m.env, m.activity);
    FileReaderAndroidPtr reader = FileReaderAndroid::Create();
    reader->Init(m.env, m.assets, classLoader);
    m.context->SetFileReader(reader);
    ModelLoaderAndroidSynchronizerObserverPtr finalizer = ModelLoaderAndroidSynchronizerObserver::Create();
    ContextSynchronizerObserverPtr obs = finalizer;
    m.context->RegisterContextSynchronizerObserver(obs);
    NodeFactoryObjPtr factory = NodeFactoryObj::Create(m.context);
    ParserObjPtr parser = ParserObj::Create(m.context);
    parser->SetFileReader(reader);
    parser->SetObserver(factory);

    bool done = false;
    while (!done) {
      std::vector<LoadInfo> list;
      {
        MutexAutoLock lock(m.loadLock);
        list.swap(m.loadList);
        done = m.done;
        while (list.size() == 0 && !done) {
          m.loadLock.Wait();
          list.swap(m.loadList);
          done = m.done;
        }
      }

      if (!done) {
        for (LoadInfo& info: list) {
          GroupPtr group = Group::Create(m.context);
          finalizer->Set(group, info.target, info.callback);
          factory->SetModelRoot(group);
          parser->LoadModel(info.name);
          if (offRenderThreadContextCurrent) {
            m.context->UpdateResourceGL();
          }
          m.context->Synchronize();
        }
      }
    }

    m.env = nullptr;

    m.context->ReleaseContextSynchronizerObserver(obs);
  }
  if (attached) {
    m.jvm->DetachCurrentThread();
  }
  {
    MutexAutoLock(m.loadLock);
    m.quitting = true;
  }
  VRB_LOG("ModelLoaderAndroid load thread stopping");
  return nullptr;
}

ModelLoaderAndroid::ModelLoaderAndroid(State& aState, RenderContextPtr& aContext)
    : m(aState) {
  m.context = CreationContext::Create(aContext);
  m.render = aContext;
}

ModelLoaderAndroid::~ModelLoaderAndroid() {
  m.StopThread();
}


} // namespace vrb
