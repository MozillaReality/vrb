/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/RenderState.h"
#include "vrb/private/ResourceGLState.h"

#include "vrb/BasicShaders.h"
#include "vrb/Color.h"
#include "vrb/ConcreteClass.h"
#include "vrb/Logger.h"
#include "vrb/GLError.h"
#include "vrb/Matrix.h"
#include "vrb/Program.h"
#include "vrb/ShaderUtil.h"
#include "vrb/Texture.h"
#include "vrb/Vector.h"

#include "vrb/gl.h"
#include <string>
#include <vector>
#include <vrb/ProgramFactory.h>

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

  ProgramPtr program;
  bool updateProgram;
  GLint uPerspective;
  GLint uView;
  GLint uModel;
  GLint uUVTransform;
  GLint uLightCount;
  ULight uLights[VRB_MAX_LIGHTS];
  GLint uMatterialAmbient;
  GLint uMatterialDiffuse;
  GLint uMatterialSpecular;
  GLint uMatterialSpecularExponent;
  GLint uTexture0;
  GLint uTintColor;
  GLint aPosition;
  GLint aNormal;
  GLint aUV;
  GLint aColor;
  std::vector<Light> lights;
  Color ambient;
  Color diffuse;
  Color specular;
  float specularExponent;
  TexturePtr texture;
  Color tintColor;
  uint32_t lightId;
  bool lightsEnabled;
  bool uvTransformEnabled;
  vrb::Matrix uvTransform;
  std::string customFragmentShader;

  State()
      : program(0)
      , updateProgram(true)
      , uPerspective(-1)
      , uView(-1)
      , uModel(-1)
      , uUVTransform(-1)
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
      , aColor(-1)
      , specularExponent(0.0f)
      , ambient(0.5f, 0.5f, 0.5f, 1.0f) // default to gray
      , diffuse(1.0f, 1.0f, 1.0f, 1.0f) // default to white
      , tintColor(1.0f, 1.0f, 1.0f, 1.0f)
      , lightId(0)
      , lightsEnabled(true)
      , uvTransformEnabled(false)
      , uvTransform(Matrix::Identity())
  {}

  void InitializeProgram();
};

void
RenderState::State::InitializeProgram() {
  if (!program || program->GetProgram() == 0) {
    return;
  }
  const bool kEnableTexturing = texture != nullptr;
  uvTransformEnabled = program->SupportsFeatures(FeatureUVTransform);

  uPerspective = program->GetUniformLocation("u_perspective");
  uView = program->GetUniformLocation("u_view");
  uModel = program->GetUniformLocation("u_model");
  uLightCount = program->GetUniformLocation("u_lightCount");
  if (uvTransformEnabled) {
    uUVTransform = program->GetUniformLocation("u_uv_transform");
  }
  const std::string structNameOpen("u_lights[");
  const std::string structNameClose("].");
  const std::string directionName("direction");
  const std::string ambientName("ambient");
  const std::string diffuseName("diffuse");
  const std::string specularName("specular");
  for (int ix = 0; ix < VRB_MAX_LIGHTS; ix++) {
    const std::string structName = structNameOpen + std::to_string(ix) + structNameClose;
    const std::string direction = structName + directionName;
    const std::string ambient = structName + ambientName;
    const std::string diffuse = structName + diffuseName;
    const std::string specular = structName + specularName;
    uLights[ix].direction = program->GetUniformLocation(direction);
    uLights[ix].ambient = program->GetUniformLocation(ambient);
    uLights[ix].diffuse = program->GetUniformLocation(diffuse);
    uLights[ix].specular = program->GetUniformLocation(specular);
  }
  const std::string materialName("u_material.");
  const std::string specularExponentName("specularExponent");
  const std::string ambient = materialName + ambientName;
  const std::string diffuse = materialName + diffuseName;
  const std::string specular = materialName + specularName;
  const std::string specularExponent = materialName + specularExponentName;
  uMatterialAmbient = program->GetUniformLocation(ambient);
  uMatterialDiffuse = program->GetUniformLocation(diffuse);
  uMatterialSpecular = program->GetUniformLocation(specular);
  uMatterialSpecularExponent = program->GetUniformLocation(specularExponent);
  if (kEnableTexturing) {
    const std::string texture0("u_texture0");
    uTexture0 = program->GetUniformLocation(texture0);
  }
  uTintColor = program->GetUniformLocation("u_tintColor");
  aPosition = program->GetAttributeLocation("a_position");
  aNormal = program->GetAttributeLocation("a_normal");
  if (kEnableTexturing) {
    aUV = program->GetAttributeLocation("a_uv");
  }
  if (program->SupportsFeatures(FeatureVertexColor)) {
    aColor = program->GetAttributeLocation("a_color");
  }
  updateProgram = false;
}

RenderStatePtr
RenderState::Create(CreationContextPtr& aContext) {
  return std::make_shared<ConcreteClass<RenderState, RenderState::State>>(aContext);
}

void
RenderState::SetProgram(ProgramPtr& aProgram) {
  m.program = aProgram;
  m.updateProgram = true;
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

GLint
RenderState::AttributeColor() const {
  return m.aColor;
}

uint32_t
RenderState::GetLightId() const {
  return m.lightId;
}

void
RenderState::ResetLights(const uint32_t aId) {
  m.lightId = aId;
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
}


void
RenderState::SetAmbient(const Color& aColor) {
  m.ambient = aColor;
}

void
RenderState::SetDiffuse(const Color& aColor) {
  m.diffuse = aColor;
}

void
RenderState::GetMaterial(Color& aAmbient, Color& aDiffuse, Color& aSpecular, float& aSpecularExponent) const {
  aAmbient = m.ambient;
  aDiffuse = m.diffuse;
  aSpecular = m.specular;
  aSpecularExponent = m.specularExponent;
}

GLint
RenderState::UVLength() const {
  if (!m.texture) {
    return 0;
  }
  return m.texture->GetTarget() == GL_TEXTURE_CUBE_MAP ? 3 : 2;
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
  if (!m.program->Enable()) { return false; }
  if (m.updateProgram) {
    m.InitializeProgram();
  }

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

  VRB_GL_CHECK(glUniform4fv(m.uMatterialAmbient, 1, m.ambient.Data()));
  VRB_GL_CHECK(glUniform4fv(m.uMatterialDiffuse, 1, m.diffuse.Data()));
  VRB_GL_CHECK(glUniform4fv(m.uMatterialSpecular, 1, m.specular.Data()));
  VRB_GL_CHECK(glUniform1f(m.uMatterialSpecularExponent, m.specularExponent));

  if (m.texture) {
    VRB_GL_CHECK(glActiveTexture(GL_TEXTURE0));
    m.texture->Bind();
    VRB_GL_CHECK(glUniform1i(m.uTexture0, 0));
  }
  VRB_GL_CHECK(glUniform4f(m.uTintColor, m.tintColor.Red(), m.tintColor.Green(), m.tintColor.Blue(), m.tintColor.Alpha()));
  VRB_GL_CHECK(glUniformMatrix4fv(m.uPerspective, 1, GL_FALSE, aPerspective.Data()));
  VRB_GL_CHECK(glUniformMatrix4fv(m.uView, 1, GL_FALSE, aView.Data()));
  VRB_GL_CHECK(glUniformMatrix4fv(m.uModel, 1, GL_FALSE, aModel.Data()));
  if (m.uvTransformEnabled) {
    VRB_GL_CHECK(glUniformMatrix4fv(m.uUVTransform, 1, GL_FALSE, m.uvTransform.Data()));
  }
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

void
RenderState::SetUVTransform(const vrb::Matrix& aMatrix) {
  m.uvTransform = aMatrix;
}

RenderState::RenderState(State& aState, CreationContextPtr& aContext) : ResourceGL(aState, aContext), m(aState) {}

void
RenderState::InitializeGL() {
}

void
RenderState::ShutdownGL() {
  m.updateProgram = true;
}

} // namespace vrb
