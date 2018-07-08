/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/SurfaceTextureFactory.h"
#include "vrb/private/ResourceGLState.h"
#include "vrb/private/UpdatableState.h"
#include "vrb/ConcreteClass.h"
#include "vrb/GLError.h"
#include "vrb/Logger.h"

#include <forward_list>
#include <memory>

namespace {

struct SurfaceTextureRecord {
  std::string name;
  vrb::SurfaceTextureObserverPtr observer;
  jobject surface;
  GLuint texture;
  bool attached;

  SurfaceTextureRecord() : surface(nullptr), texture(0) {}
  SurfaceTextureRecord(const std::string& aName, const vrb::SurfaceTextureObserverPtr& aObserver)
      : name(aName)
      , observer(aObserver)
      , surface(nullptr)
      , texture(0)
      , attached(false)
  {}
  SurfaceTextureRecord(SurfaceTextureRecord&& aRecord)
      : name(aRecord.name)
      , observer(std::move(aRecord.observer))
      , surface(aRecord.surface)
      , texture(aRecord.texture)
  {}

  SurfaceTextureRecord& operator=(SurfaceTextureRecord&& aRecord) {
    name = aRecord.name;
    observer = std::move(aRecord.observer);
    surface = aRecord.surface;
    texture = aRecord.texture;
    return *this;
  }

  void Release(JNIEnv* aEnv);
};

void
SurfaceTextureRecord::Release(JNIEnv* aEnv) {
  if (observer) {
    observer->SurfaceTextureDestroyed(name);
  }
  if (aEnv && surface) {
    aEnv->DeleteGlobalRef(surface);
    surface = nullptr;
  }
  if (texture) {
    VRB_GL_CHECK(glDeleteTextures(1, &texture));
    texture = 0;
  }
}

}

namespace vrb {

struct SurfaceTextureFactory::State : public Updatable::State, public ResourceGL::State {
  JNIEnv* env;
  jclass surfaceTextureClass;
  jmethodID surfaceTextureCtor;
  jmethodID updateTexImageMethod;
  jmethodID attachToGLContextMethod;
  jmethodID detachFromGLContextMethod;
  std::forward_list<SurfaceTextureRecord> textures;
  std::forward_list<SurfaceTextureObserverPtr> observers;

  State()
      : env(nullptr)
      , surfaceTextureClass(nullptr)
      , surfaceTextureCtor(nullptr)
      , updateTexImageMethod(nullptr)
      , attachToGLContextMethod(nullptr)
      , detachFromGLContextMethod(nullptr)
  {}

  bool Contains(const std::string& aName);
  void Initialize(JNIEnv* aEnv);
  void Shutdown();
};

bool
SurfaceTextureFactory::State::Contains(const std::string& aName) {
  for (SurfaceTextureRecord& record: textures) {
    if (record.name == aName) {
      return true;
    }
  }
  return false;
}

void
SurfaceTextureFactory::State::Initialize(JNIEnv* aEnv) {
  env = aEnv;
  if (!env) {
    return;
  }

  jclass localSurfaceTextureFactoryClass = env->FindClass("android/graphics/SurfaceTexture");
  surfaceTextureClass = (jclass)env->NewGlobalRef(localSurfaceTextureFactoryClass);

  if (!surfaceTextureClass) {
    VRB_LOG("Failed finding Android class type: android/graphics/SurfaceTexture");
    return;
  }

  surfaceTextureCtor = env->GetMethodID(surfaceTextureClass, "<init>", "(I)V");

  if (!surfaceTextureCtor) {
    VRB_LOG("Failed finding SurfaceTexure <init> function");
  }

  updateTexImageMethod = env->GetMethodID(surfaceTextureClass, "updateTexImage", "()V");

  if (!updateTexImageMethod) {
    VRB_LOG("Failed finding SurfaceTexure.updateTexImage function");
  }

  attachToGLContextMethod = env->GetMethodID(surfaceTextureClass, "attachToGLContext", "(I)V");

  if (!attachToGLContextMethod) {
    VRB_LOG("Failed finding SurfaceTexure.attachToGLContext function");
  }

  detachFromGLContextMethod = env->GetMethodID(surfaceTextureClass, "detachFromGLContext", "()V");

  if (!detachFromGLContextMethod) {
    VRB_LOG("Failed finding SurfaceTexure.detachFromGLContext function");
  }
}

void
SurfaceTextureFactory::State::Shutdown() {
  if (!env) {
    return;
  }
  for (SurfaceTextureRecord& record: textures) {
    record.Release(env);
  }
  env->DeleteGlobalRef(surfaceTextureClass);
  surfaceTextureClass = nullptr;
  surfaceTextureCtor = nullptr;
  updateTexImageMethod = nullptr;
  attachToGLContextMethod = nullptr;
  detachFromGLContextMethod = nullptr;
  env = nullptr;
}

SurfaceTextureFactoryPtr
SurfaceTextureFactory::Create(CreationContextPtr& aContext) {
  return std::make_shared<ConcreteClass<SurfaceTextureFactory, SurfaceTextureFactory::State> >(aContext);
}

void
SurfaceTextureFactory::InitializeJava(JNIEnv* aEnv) {
  m.Initialize(aEnv);
}

void
SurfaceTextureFactory::ShutdownJava() {
  m.Shutdown();
}

void
SurfaceTextureFactory::CreateSurfaceTexture(const std::string& aName, SurfaceTextureObserverPtr aObserver) {
  if (m.Contains(aName)) {
    return;
  }

  SurfaceTextureRecord record{aName, aObserver};
  m.textures.push_front(std::move(record));
}

void
SurfaceTextureFactory::DestroySurfaceTexture(const std::string& aName) {
  m.textures.remove_if([&m = this->m, &aName](SurfaceTextureRecord& aRecord) -> bool {
    if (aRecord.name == aName) {
      for (SurfaceTextureObserverPtr& observer: m.observers) {
        observer->SurfaceTextureDestroyed(aRecord.name);
      }
      aRecord.Release(m.env);
      return true;
    }
    return false;
  });
}

void
SurfaceTextureFactory::AddGlobalObserver(SurfaceTextureObserverPtr aObserver) {
  m.observers.push_front(std::move(aObserver));
}

void
SurfaceTextureFactory::RemoveGlobalObserver(const SurfaceTextureObserver& aObserver) {
  const SurfaceTextureObserver* observer = &aObserver;
  m.observers.remove_if([observer](SurfaceTextureObserverPtr& aObserver) -> bool {
    return observer == aObserver.get();
  });
}

jobject
SurfaceTextureFactory::LookupSurfaceTexture(const std::string& aName) {
  for (SurfaceTextureRecord& record: m.textures) {
    if (aName == record.name) {
      return record.surface;
    }
  }
  return nullptr;
}

SurfaceTextureFactory::SurfaceTextureFactory(State& aState, CreationContextPtr& aContext)
    : Updatable(aState, aContext)
    , ResourceGL(aState, aContext)
    , m(aState) {}

SurfaceTextureFactory::~SurfaceTextureFactory() {}

void
SurfaceTextureFactory::UpdateResource(RenderContext& aContext) {
  if (!m.env) {
    return;
  }
  for(SurfaceTextureRecord& record: m.textures) {
    if (!record.surface) {
      if (!record.texture) {
        VRB_GL_CHECK(glGenTextures(1, &record.texture));
      }
      if (!record.texture) {
        const std::string reason("glGenTextures failed to return valid texture handle.");
        if (record.observer) {
          record.observer->SurfaceTextureCreationError(record.name, reason);
        }
        for (SurfaceTextureObserverPtr& observer: m.observers) {
          observer->SurfaceTextureCreationError(record.name, reason);
        }
        continue;
      }
      if (m.surfaceTextureClass && m.surfaceTextureCtor) {
        jobject localSurface =  m.env->NewObject(m.surfaceTextureClass, m.surfaceTextureCtor, (int)record.texture);
        if (!localSurface) {
          const std::string reason("NewObject failed to return valid jobject for SurfaceTexture.");
          if (record.observer) {
            record.observer->SurfaceTextureCreationError(record.name, reason);
          }
          for (SurfaceTextureObserverPtr& observer: m.observers) {
            observer->SurfaceTextureCreationError(record.name, reason);
          }
          continue;
        }
        record.surface = m.env->NewGlobalRef(localSurface);
        record.attached = true;
        m.env->DeleteLocalRef(localSurface);
        if (record.observer) {
          record.observer->SurfaceTextureCreated(record.name, record.texture, record.surface);
        }
        for (SurfaceTextureObserverPtr& observer: m.observers) {
          observer->SurfaceTextureCreated(record.name, record.texture, record.surface);
        }
      }
    }
    if (record.surface && record.attached && m.updateTexImageMethod) {
      m.env->CallVoidMethod(record.surface, m.updateTexImageMethod);
    } else if (record.surface && !record.attached) {
      VRB_LOG("********* SurfaceTexture not active!");
    }
  }
}

// ResourceGL interface
void
SurfaceTextureFactory::InitializeGL() {
  VRB_LOG("***** SurfaceTextureFactory::InitializeGL");
  for(SurfaceTextureRecord& record: m.textures) {
    if (record.surface && !record.attached) {
      VRB_GL_CHECK(glGenTextures(1, &record.texture));
      m.env->CallVoidMethod(record.surface, m.attachToGLContextMethod, record.texture);
      record.attached = true;
      if (record.observer) {
        record.observer->SurfaceTextureHandleUpdated(record.name, record.texture);
      }
      for (SurfaceTextureObserverPtr& observer: m.observers) {
        observer->SurfaceTextureHandleUpdated(record.name, record.texture);
      }
    }
  }
}

void
SurfaceTextureFactory::ShutdownGL() {
  VRB_LOG("***** SurfaceTextureFactory::ShutdownGL");
  for(SurfaceTextureRecord& record: m.textures) {
    if (record.surface && record.attached) {
      m.env->CallVoidMethod(record.surface, m.detachFromGLContextMethod);
      record.texture = 0;
      record.attached = false;
      if (record.observer) {
        record.observer->SurfaceTextureHandleUpdated(record.name, record.texture);
      }
      for (SurfaceTextureObserverPtr& observer: m.observers) {
        observer->SurfaceTextureHandleUpdated(record.name, record.texture);
      }
    }
  }
}

} // namespace vrb
