/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "vrb/ProgramFactory.h"

#include "vrb/BasicShaders.h"
#include "vrb/ConcreteClass.h"
#include "vrb/CreationContext.h"
#include "vrb/LoaderThread.h"
#include <vrb/Mutex.h>
#include "vrb/ResourceGL.h"
#include "vrb/private/ResourceGLState.h"

#include <string>
#include <unordered_map>
#include <vrb/Program.h>
#include <vrb/GLError.h>
#include <vrb/ShaderUtil.h>

namespace vrb {

class ProgramBuilder;
typedef std::shared_ptr<ProgramBuilder> ProgramBuilderPtr;

class ProgramBuilder : public ResourceGL {
public:
  static ProgramBuilderPtr Create(LoaderThreadWeak aLoader);

  ProgramPtr GetProgram();
  void SetFeatures(const uint32_t aFeatureMask, const std::string& aCustomFragShader);
  void Finalize();

  // ResourceGL Interface
  bool SupportOffRenderThreadInitialization() override;
  void InitializeGL() override;
  void ShutdownGL() override;

protected:
  struct State;
  ProgramBuilder(State& aState);

private:
  State& m;
};

struct ProgramBuilder::State : public ResourceGL::State {
  LoaderThreadWeak loaderHandle;
  ProgramPtr program;
  uint32_t featureMask;
  std::string customFragmentShader;
  GLuint vertexShader;
  GLuint fragmentShader;
  GLuint programHandle;

  State() : program(Program::Create()), featureMask(0), vertexShader(0), fragmentShader(0), programHandle(0) {}
  bool IsTexturingEnabled() { return (featureMask & (FeatureTexture | FeatureCubeTexture | FeatureSurfaceTexture)) != 0; }
  bool IsCubeMapTextureEnabled() { return (featureMask & FeatureCubeTexture) != 0; }
  bool IsSurfaceTextureEnabled() { return (featureMask & FeatureSurfaceTexture) != 0;}
  std::string GetFragmentShader(const std::string& aSource) {
    const std::string kPrecisionMacro("VRB_FRAGMENT_PRECISION");
    std::string result = aSource;
    const size_t kUVStart = result.find(kPrecisionMacro);
    if (kUVStart != std::string::npos) {
      const char * precision;
      if ((featureMask & FeatureHighPrecision) != 0) {
        precision = "highp";
      } else if ((featureMask & FeatureLowPrecision) != 0) {
        precision = "lowp";
      } else {
        precision = "mediump";
      }
      result.replace(kUVStart, kPrecisionMacro.length(), precision);
    }
    return result;
  }
};

ProgramBuilderPtr
ProgramBuilder::Create(LoaderThreadWeak aLoader) {
  ProgramBuilderPtr result = std::make_shared<ConcreteClass<ProgramBuilder, ProgramBuilder::State> >();
  result->m.loaderHandle = aLoader;
  return result;
}


ProgramPtr
ProgramBuilder::GetProgram() {
  return m.program;
}

void
ProgramBuilder::SetFeatures(const uint32_t aFeatureMask, const std::string& aCustomFragShader) {
  m.featureMask = aFeatureMask;
  m.customFragmentShader = aCustomFragShader;
  m.program->SetFeatures(aFeatureMask);
}


void
ProgramBuilder::Finalize() {
  if (!m.program) {
    return;
  }
  m.program->SetProgram(m.programHandle);
}

bool
ProgramBuilder::SupportOffRenderThreadInitialization() { return true; }

void
ProgramBuilder::InitializeGL() {
  std::string vertexShaderSource = GetVertexShaderSource();
  const std::string kTextureUVMacro("VRB_TEXTURE_UV_TYPE");
  const size_t kUVStart = vertexShaderSource.find(kTextureUVMacro);
  if (kUVStart != std::string::npos) {
    vertexShaderSource.replace(kUVStart, kTextureUVMacro.length(), m.IsCubeMapTextureEnabled() ? "vec3" : "vec2");
  }

  const std::string kUVTransformMacro("VRB_UV_TRANSFORM_ENABLED");
  const size_t kUVTransformStart = vertexShaderSource.find(kUVTransformMacro);
  if (kUVTransformStart != std::string::npos) {
    vertexShaderSource.replace(kUVTransformStart, kUVTransformMacro.length(), (m.featureMask & FeatureUVTransform) != 0 ? "1" : "0");
  }

  const std::string kVertexColorMacro("VRB_VERTEX_COLOR_ENABLED");
  const size_t kVertexColorStart = vertexShaderSource.find(kVertexColorMacro);
  if (kVertexColorStart != std::string::npos) {
    vertexShaderSource.replace(kVertexColorStart, kVertexColorMacro.length(), (m.featureMask & FeatureVertexColor) != 0 ? "1" : "0");
  }

  const std::string kTextureMacro("VRB_TEXTURE_STATE");
  const size_t kStart = vertexShaderSource.find(kTextureMacro);
  if (m.IsTexturingEnabled()) {
    if(kStart != std::string::npos) {
      vertexShaderSource.replace(kStart, kTextureMacro.length(), "1");
    }
    m.vertexShader = LoadShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
    std::string frag = m.GetFragmentShader(GetFragmentTextureShaderSource());
    if (m.IsCubeMapTextureEnabled()) {
      frag = m.GetFragmentShader(GetFragmentCubeMapTextureShaderSource());
    }
#if defined(ANDROID)
    // SurfaceTexture requires usage of fragment shader extension.
    if (m.IsSurfaceTextureEnabled()) {
      frag = m.GetFragmentShader(GetFragmentSurfaceTextureShaderSource());
    }
#endif // defined(ANDROID)
    if (!m.customFragmentShader.empty()) {
      frag = m.GetFragmentShader(m.customFragmentShader);
    }
    m.fragmentShader = LoadShader(GL_FRAGMENT_SHADER, frag.c_str());
  } else {
    if(kStart != std::string::npos) {
      vertexShaderSource.replace(kStart, kTextureMacro.length(), "0");
    }
    m.vertexShader = LoadShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
    std::string frag = m.GetFragmentShader(GetFragmentShaderSource());
    if (!m.customFragmentShader.empty()) {
      frag = m.GetFragmentShader(m.customFragmentShader);
    }
    m.fragmentShader = LoadShader(GL_FRAGMENT_SHADER, frag.c_str());
  }
  if (m.fragmentShader && m.vertexShader) {
    m.programHandle = CreateProgram(m.vertexShader, m.fragmentShader);
  }

  LoaderThreadPtr loader = m.loaderHandle.lock();
  if (!loader || !loader->IsOnLoaderThread() ) {
    Finalize();
  }
}

void
ProgramBuilder::ShutdownGL() {
  if (m.programHandle) {
    VRB_GL_CHECK(glDeleteProgram(m.programHandle));
    m.programHandle = 0;
    m.program->SetProgram(0);
  }

  if (m.vertexShader) {
    VRB_GL_CHECK(glDeleteShader(m.vertexShader));
    m.vertexShader = 0;
  }

  if (m.fragmentShader) {
    VRB_GL_CHECK(glDeleteShader(m.fragmentShader));
    m.fragmentShader = 0;
  }
}

ProgramBuilder::ProgramBuilder(State& aState) : ResourceGL(aState), m(aState) {}

struct ProgramFactory::State {
  Mutex lock;
  std::unordered_map<std::string, ProgramBuilderPtr> programs;
  LoaderThreadWeak loader;
};

ProgramFactoryPtr
ProgramFactory::Create() {
  ProgramFactoryPtr result = std::make_shared<ConcreteClass<ProgramFactory, ProgramFactory::State> >();
  return result;
}

void
ProgramFactory::SetLoaderThread(LoaderThreadPtr aLoader) {
  m.loader = aLoader;
}

ProgramPtr
ProgramFactory::CreateProgram(CreationContextPtr& aContext, uint32_t aFeatureMask) {
  static const std::string empty;
  return CreateProgram(aContext, aFeatureMask, empty);
}

ProgramPtr
ProgramFactory::CreateProgram(CreationContextPtr& aContext, const uint32_t aFeatureMask,
                              const std::string& aCustomFragShader) {
  const std::string key = std::to_string(aFeatureMask) + aCustomFragShader;
  ProgramBuilderPtr builder;
  bool created = false;
  {
    MutexAutoLock lock(m.lock);

    auto found = m.programs.find(key);
    if (found == m.programs.end()) {
      builder = ProgramBuilder::Create(m.loader);
      builder->SetFeatures(aFeatureMask, aCustomFragShader);
      m.programs[key] = builder;
      created = true;
    } else {
      builder = found->second;
    }
  }

  if (created) {
    LoaderThreadPtr loader = m.loader.lock();
    if (loader) {
      LoadFinishedCallback finished = [builder](GroupPtr&) {
        builder->Finalize();
      };
      if (loader->IsOnLoaderThread()) {
        loader->AddFinishedCallback(finished);
        aContext->AddResourceGL(builder.get());
      } else {

        LoadTask task = [builder](CreationContextPtr& aContext) -> GroupPtr {
          aContext->AddResourceGL(builder.get());
          return nullptr;
        };

        loader->RunLoadTask(nullptr, task, finished);
      }
    } else {
      aContext->AddResourceGL(builder.get());
    }
  }

  return builder->GetProgram();
}

ProgramFactory::ProgramFactory(State& aState) : m(aState) {}

}
