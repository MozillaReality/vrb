/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/RenderState.h"
#include "vrb/private/ResourceGLState.h"

#include "vrb/Color.h"
#include "vrb/ConcreteClass.h"
#include "vrb/Logger.h"
#include "vrb/GLError.h"
#include "vrb/Matrix.h"
#include "vrb/ShaderUtil.h"
#include "vrb/Texture.h"
#if defined(ANDROID)
#include "vrb/TextureSurface.h"
#endif // defined(ANDROID)
#include "vrb/Vector.h"

#include "vrb/gl.h"
#include <string>
#include <vector>

namespace {

// make part of shader generation.
static const int MaxLights = 2;

static const char* sVertexShaderSource = R"SHADER(

#define MAX_LIGHTS 2
#define VRB_USE_TEXTURE VRB_TEXTURE_STATE
#define VRB_UV_TYPE VRB_TEXTURE_UV_TYPE

struct Light {
  vec3 direction;
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
};

struct Material {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float specularExponent;
};

uniform mat4 u_perspective;
uniform mat4 u_view;
uniform mat4 u_model;
uniform int u_lightCount;
uniform Light u_lights[MAX_LIGHTS];
uniform Material u_material;
uniform vec4 u_tintColor;

attribute vec3 a_position;
attribute vec3 a_normal;

varying vec4 v_color;

#ifdef VRB_USE_TEXTURE
attribute VRB_UV_TYPE a_uv;
varying VRB_UV_TYPE v_uv;
#endif // VRB_USE_TEXTURE

vec4 normal;

vec4
calculate_light(int index) {
  vec4 result = vec4(0, 0, 0, 0);
  vec4 direction = -normalize(u_view * vec4(u_lights[index].direction.xyz, 0));
  vec4 hvec;
  float ndotl;
  float ndoth;
  result += u_lights[index].ambient * u_material.ambient;
  ndotl = max(0.0, dot(normal, direction));
  result += (ndotl * u_lights[index].diffuse * u_material.diffuse);
  hvec = normalize(direction + vec4(0.0, 0.0, 1.0, 0.0));
  ndoth = dot(normal, hvec);
  if (ndoth > 0.0) {
    result += (pow(ndoth, u_material.specularExponent) * u_material.specular * u_lights[index].specular);
  }
  return result;
}

void main(void) {
  int ix;
  v_color = vec4(0, 0, 0, 0);
  normal = normalize(u_view * u_model * vec4(a_normal.xyz, 0));
  for(ix = 0; ix < MAX_LIGHTS; ix++) {
    if (ix >= u_lightCount) {
      break;
    }
    v_color += calculate_light(ix);
    v_color.a = u_material.diffuse.a;
  }
  if (u_lightCount == 0) {
    v_color = u_material.diffuse;
  }
  v_color *= u_tintColor;
#ifdef VRB_USE_TEXTURE
  v_uv = a_uv;
#endif // VRB_USE_TEXTURE
  gl_Position = u_perspective * u_view * u_model * vec4(a_position.xyz, 1);
}

)SHADER";

static const char* sFragmentShaderSource = R"SHADER(
precision mediump float;

varying vec4 v_color;

void main() {
  gl_FragColor = v_color;
}

)SHADER";

static const char* sFragmentTextureShaderSource = R"SHADER(
precision mediump float;

uniform sampler2D u_texture0;
varying vec4 v_color;
varying vec2 v_uv;

void main() {
  gl_FragColor = texture2D(u_texture0, v_uv) * v_color;
}

)SHADER";

static const char* sFragmentSurfaceTextureShaderSource = R"SHADER(
#extension GL_OES_EGL_image_external : require
precision mediump float;

uniform samplerExternalOES u_texture0;
varying vec4 v_color;
varying vec2 v_uv;

void main() {
  gl_FragColor = texture2D(u_texture0, v_uv) * v_color;
}

)SHADER";

static const char* sFragmentCubeMapTextureShaderSource = R"SHADER(
precision mediump float;

uniform samplerCube u_texture0;
varying vec4 v_color;
varying vec3 v_uv;

void main() {
  gl_FragColor = textureCube(u_texture0, v_uv) * v_color;
}

)SHADER";

}

namespace vrb {

struct RenderState::State : public ResourceGL::State {
  struct Light {
    const Vector direction;
    const Color ambient;
    const Color diffuse;
    const Color specular;

    Light(const Vector& aDirection, const Color& aAmbient, const Color& aDiffuse, const Color& aSpecular)
        : direction(aDirection)
        , ambient(aAmbient)
        , diffuse(aDiffuse)
        , specular(aSpecular)
    {}
  };
  struct ULight {
    GLint direction;
    GLint ambient;
    GLint diffuse;
    GLint specular;
    ULight()
        : direction(0)
        , ambient(0)
        , diffuse(0)
        , specular(0)
    {}
  };
  GLuint vertexShader;
  GLuint fragmentShader;
  GLuint program;
  GLint uPerspective;
  GLint uView;
  GLint uModel;
  GLint uLightCount;
  ULight uLights[MaxLights];
  GLint uMatterialAmbient;
  GLint uMatterialDiffuse;
  GLint uMatterialSpecular;
  GLint uMatterialSpecularExponent;
  GLint uTexture0;
  GLint uTintColor;
  GLint aPosition;
  GLint aNormal;
  GLint aUV;
  std::vector<Light> lights;
  Color ambient;
  Color diffuse;
  Color specular;
  float specularExponent;
  TexturePtr texture;
  Color tintColor;
  uint32_t lightId;
  bool updateLights;
  bool updateMaterial;
  bool lightsEnabled;

  State()
      : vertexShader(0)
      , fragmentShader(0)
      , program(0)
      , uPerspective(-1)
      , uView(-1)
      , uModel(-1)
      , uLightCount(-1)
      , uMatterialAmbient(-1)
      , uMatterialDiffuse(-1)
      , uMatterialSpecular(-1)
      , uMatterialSpecularExponent(-1)
      , uTexture0(-1)
      , uTintColor(-1)
      , aPosition(-1)
      , aNormal(-1)
      , aUV(-1)
      , specularExponent(0.0f)
      , ambient(0.5f, 0.5f, 0.5f, 1.0f) // default to gray
      , diffuse(1.0f, 1.0f, 1.0f, 1.0f) // default to white
      , tintColor(1.0f, 1.0f, 1.0f, 1.0f)
      , lightId(0)
      , updateLights(false)
      , updateMaterial(true)
      , lightsEnabled(true)
  {}
};

RenderStatePtr
RenderState::Create(CreationContextPtr& aContext) {
  return std::make_shared<ConcreteClass<RenderState, RenderState::State>>(aContext);
}

GLuint
RenderState::Program() const {
  return m.program;
}

GLint
RenderState::AttributePosition() const {
  return m.aPosition;
}

GLint
RenderState::AttributeNormal() const {
  return m.aNormal;
}

GLint
RenderState::AttributeUV() const {
  return m.aUV;
}

uint32_t
RenderState::GetLightId() const {
  return m.lightId;
}

void
RenderState::ResetLights(const uint32_t aId) {
  m.lightId = aId;
  m.updateLights = true;
  m.lights.clear();
}

void
RenderState::AddLight(const Vector& aDirection, const Color& aAmbient, const Color& aDiffuse, const Color& aSpecular) {
  m.lights.push_back(State::Light(aDirection, aAmbient, aDiffuse, aSpecular));
}

void
RenderState::SetMaterial(const Color& aAmbient, const Color& aDiffuse, const Color& aSpecular, const float aSpecularExponent) {
  m.ambient = aAmbient;
  m.diffuse = aDiffuse;
  m.specular = aSpecular;
  m.specularExponent = aSpecularExponent;
  m.updateMaterial = true;
}


void
RenderState::SetAmbient(const Color& aColor) {
  m.ambient = aColor;
  m.updateMaterial = true;
}

void
RenderState::SetDiffuse(const Color& aColor) {
  m.diffuse = aColor;
  m.updateMaterial = true;
}

void
RenderState::GetMaterial(Color& aAmbient, Color& aDiffuse, Color& aSpecular, float& aSpecularExponent) const {
  aAmbient = m.ambient;
  aDiffuse = m.diffuse;
  aSpecular = m.specular;
  aSpecularExponent = m.specularExponent;
}

TexturePtr
RenderState::GetTexture() const {
  return m.texture;
}

void
RenderState::SetTexture(const TexturePtr& aTexture) {
  m.texture = aTexture;
}

bool
RenderState::HasTexture() const {
  return m.texture != nullptr;
}

const Color&
RenderState::GetTintColor() const {
  return m.tintColor;
}

void
RenderState::SetTintColor(const Color& aColor) {
  m.tintColor = aColor;
}

bool
RenderState::Enable(const Matrix& aPerspective, const Matrix& aView, const Matrix& aModel) {
  if (!m.program) { return false; }
  VRB_GL_CHECK(glUseProgram(m.program));
  if (m.updateLights) {
    m.updateLights = false;
    int lightCount = 0;
    if (m.lightsEnabled) {
      for (State::Light& light: m.lights) {
        VRB_GL_CHECK(glUniform3f(m.uLights[lightCount].direction, light.direction.x(), light.direction.y(), light.direction.z()));
        VRB_GL_CHECK(glUniform4fv(m.uLights[lightCount].ambient, 1, light.ambient.Data()));
        VRB_GL_CHECK(glUniform4fv(m.uLights[lightCount].diffuse, 1, light.diffuse.Data()));
        VRB_GL_CHECK(glUniform4fv(m.uLights[lightCount].specular, 1, light.specular.Data()));
        lightCount++;
      }
    }
    VRB_GL_CHECK(glUniform1i(m.uLightCount, lightCount));
  }
  if (m.updateMaterial) {
    m.updateMaterial = false;
     VRB_GL_CHECK(glUniform4fv(m.uMatterialAmbient, 1, m.ambient.Data()));
     VRB_GL_CHECK(glUniform4fv(m.uMatterialDiffuse, 1, m.diffuse.Data()));
     VRB_GL_CHECK(glUniform4fv(m.uMatterialSpecular, 1, m.specular.Data()));
     VRB_GL_CHECK(glUniform1f(m.uMatterialSpecularExponent, m.specularExponent));
  }
  if (m.texture) {
    VRB_GL_CHECK(glActiveTexture(GL_TEXTURE0));
    m.texture->Bind();
    VRB_GL_CHECK(glUniform1i(m.uTexture0, 0));
  }
  VRB_GL_CHECK(glUniform4f(m.uTintColor, m.tintColor.Red(), m.tintColor.Green(), m.tintColor.Blue(), m.tintColor.Alpha()));
  VRB_GL_CHECK(glUniformMatrix4fv(m.uPerspective, 1, GL_FALSE, aPerspective.Data()));
  VRB_GL_CHECK(glUniformMatrix4fv(m.uView, 1, GL_FALSE, aView.Data()));
  VRB_GL_CHECK(glUniformMatrix4fv(m.uModel, 1, GL_FALSE, aModel.Data()));
  return true;
}

void
RenderState::Disable() {
  if (m.texture) {
    glActiveTexture(GL_TEXTURE0);
    m.texture->Unbind();
  }
}

void
RenderState::SetLightsEnabled(bool aEnabled) {
  m.lightsEnabled = aEnabled;
}

RenderState::RenderState(State& aState, CreationContextPtr& aContext) : ResourceGL(aState, aContext), m(aState) {}
RenderState::~RenderState() {}

void
RenderState::InitializeGL() {
  const bool kEnableTexturing = m.texture != nullptr;
  std::string vertexShaderSource = sVertexShaderSource;
  const std::string kTextureUVMacro("VRB_TEXTURE_UV_TYPE");
  const size_t kUVStart = vertexShaderSource.find(kTextureUVMacro);
  if (kUVStart != std::string::npos) {
    vertexShaderSource.replace(kUVStart, kTextureUVMacro.length(), m.texture && m.texture->GetTarget() == GL_TEXTURE_CUBE_MAP ? "vec3" : "vec2");
  }

  const std::string kTextureMacro("VRB_TEXTURE_STATE");
  const size_t kStart = vertexShaderSource.find(kTextureMacro);
  if (kEnableTexturing) {
    if(kStart != std::string::npos) {
      vertexShaderSource.replace(kStart, kTextureMacro.length(), "1");
    }
    m.vertexShader = LoadShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
    const char* frag = sFragmentTextureShaderSource;
    if (m.texture->GetTarget() == GL_TEXTURE_CUBE_MAP) {
      frag = sFragmentCubeMapTextureShaderSource;
    }
#if defined(ANDROID)
    // SurfaceTexture requires usage of fragment shader extension.
    if (dynamic_cast<TextureSurface*>(m.texture.get()) != nullptr) {
      frag = sFragmentSurfaceTextureShaderSource;
    }
#endif // defined(ANDROID)
    m.fragmentShader = LoadShader(GL_FRAGMENT_SHADER, frag);
  } else {
    if(kStart != std::string::npos) {
      vertexShaderSource.replace(kStart, kTextureMacro.length(), "0");
    }
    m.vertexShader = LoadShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
    m.fragmentShader = LoadShader(GL_FRAGMENT_SHADER, sFragmentShaderSource);
  }
  if (m.fragmentShader && m.vertexShader) {
    m.program = CreateProgram(m.vertexShader, m.fragmentShader);
  }
  if (m.program) {
    m.uPerspective = GetUniformLocation(m.program, "u_perspective");
    m.uView = GetUniformLocation(m.program, "u_view");
    m.uModel = GetUniformLocation(m.program, "u_model");
    m.uLightCount = GetUniformLocation(m.program, "u_lightCount");
    const std::string structNameOpen("u_lights[");
    const std::string structNameClose("].");
    const std::string directionName("direction");
    const std::string ambientName("ambient");
    const std::string diffuseName("diffuse");
    const std::string specularName("specular");
    for (int ix = 0; ix < MaxLights; ix++) {
      const std::string structName = structNameOpen + std::to_string(ix) + structNameClose;
      const std::string direction = structName + directionName;
      const std::string ambient = structName + ambientName;
      const std::string diffuse = structName + diffuseName;
      const std::string specular = structName + specularName;
      m.uLights[ix].direction = GetUniformLocation(m.program, direction);
      m.uLights[ix].ambient = GetUniformLocation(m.program, ambient);
      m.uLights[ix].diffuse = GetUniformLocation(m.program, diffuse);
      m.uLights[ix].specular = GetUniformLocation(m.program, specular);
    }
    const std::string materialName("u_material.");
    const std::string specularExponentName("specularExponent");
    const std::string ambient = materialName + ambientName;
    const std::string diffuse = materialName + diffuseName;
    const std::string specular = materialName + specularName;
    const std::string specularExponent = materialName + specularExponentName;
    m.uMatterialAmbient = GetUniformLocation(m.program, ambient);
    m.uMatterialDiffuse = GetUniformLocation(m.program, diffuse);
    m.uMatterialSpecular = GetUniformLocation(m.program, specular);
    m.uMatterialSpecularExponent = GetUniformLocation(m.program, specularExponent);
    if (kEnableTexturing) {
      const std::string texture0("u_texture0");
      m.uTexture0 = GetUniformLocation(m.program, texture0);
    }
    m.uTintColor = GetUniformLocation(m.program, "u_tintColor");
    m.aPosition = GetAttributeLocation(m.program, "a_position");
    m.aNormal = GetAttributeLocation(m.program, "a_normal");
    if (kEnableTexturing) {
      m.aUV = GetAttributeLocation(m.program, "a_uv");
    }
    m.updateMaterial = true;
  }
}

void
RenderState::ShutdownGL() {}

} // namespace vrb
