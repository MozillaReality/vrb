#include "vrb/RenderState.h"
#include "vrb/private/ResourceGLState.h"

#include "vrb/Color.h"
#include "vrb/ConcreteClass.h"
#include "vrb/Logger.h"
#include "vrb/GLError.h"
#include "vrb/Matrix.h"
#include "vrb/Vector.h"

#include <GLES2/gl2.h>
#include <string>
#include <vector>

namespace {

// make part of shader generation.
static const int MaxLights = 2;

static const char* vertexShaderSource = R"SHADER(

#define MAX_LIGHTS 2

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

attribute vec3 a_position;
attribute vec3 a_normal;

varying vec4 v_color;

vec4 normal;

vec4
calculate_light(int index) {
  vec4 result = vec4(0, 0, 0, 0);
  vec4 direction = -normalize(u_view * vec4(u_lights[index].direction.xyz, 0));
  vec4 hvec;
  float ndotl;
  float ndoth;
  result += u_lights[index].ambient * u_material.ambient;
  ndotl = max(0.0f, dot(normal, direction));
  result += (ndotl * u_lights[index].diffuse * u_material.diffuse);
  hvec = normalize(direction + vec4(0.0f, 0.0f, 1.0f, 0.0f));
  ndoth = dot(normal, hvec);
  if (ndoth > 0.0f) {
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
  gl_Position = u_perspective * u_view * u_model * vec4(a_position.xyz, 1);
}

)SHADER";

static const char* fragmentShaderSource = R"SHADER(

varying vec4 v_color;

void main() {
  gl_FragColor = v_color;
}

)SHADER";

static GLint
getUniformLocation(GLuint aProgram, const char* aName) {
  GLint result = VRB_CHECK(glGetUniformLocation(aProgram, aName));
  if (result < 0) {
    VRB_LOG("Failed to glGetUniformLocation for '%s'", aName);
  }
  return result;
}

static GLint
getUniformLocation(GLuint aProgram, const std::string& aName) {
  return getUniformLocation(aProgram, aName.c_str());
}

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
  GLint aPosition;
  GLint aNormal;
  std::vector<Light> lights;
  Color ambient;
  Color diffuse;
  Color specular;
  float specularExponent;
  uint32_t lightId;
  bool updateLights;
  bool updateMaterial;

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
      , aPosition(-1)
      , aNormal(-1)
      , specularExponent(0.0f)
      , ambient(0.5f, 0.5f, 0.5f, 1.0f) // default to gray
      , diffuse(1.0f, 1.0f, 1.0f, 1.0f) // default to white
      , lightId(0)
      , updateLights(false)
      , updateMaterial(true)
  {}
};

RenderStatePtr
RenderState::Create(ContextWeak& aContext) {
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

bool
RenderState::Enable(const Matrix& aPerspective, const Matrix& aView, const Matrix& aModel) {
  if (!m.program) { return false; }
  VRB_CHECK(glUseProgram(m.program));
  if (m.updateLights) {
    m.updateLights = false;
    int lightCount = 0;
    for (State::Light& light: m.lights) {
      VRB_CHECK(glUniform3f(m.uLights[lightCount].direction, light.direction.x(), light.direction.y(), light.direction.z()));
      VRB_CHECK(glUniform4fv(m.uLights[lightCount].ambient, 1, light.ambient.Data()));
      VRB_CHECK(glUniform4fv(m.uLights[lightCount].diffuse, 1, light.diffuse.Data()));
      VRB_CHECK(glUniform4fv(m.uLights[lightCount].specular, 1, light.specular.Data()));
      lightCount++;
    }
    VRB_CHECK(glUniform1i(m.uLightCount, lightCount));
  }
  if (m.updateMaterial) {
    m.updateMaterial = false;
     VRB_CHECK(glUniform4fv(m.uMatterialAmbient, 1, m.ambient.Data()));
     VRB_CHECK(glUniform4fv(m.uMatterialDiffuse, 1, m.diffuse.Data()));
     VRB_CHECK(glUniform4fv(m.uMatterialSpecular, 1, m.specular.Data()));
     VRB_CHECK(glUniform1f(m.uMatterialSpecularExponent, m.specularExponent));
  }
  VRB_CHECK(glUniformMatrix4fv(m.uPerspective, 1, GL_FALSE, aPerspective.Data()));
  VRB_CHECK(glUniformMatrix4fv(m.uView, 1, GL_FALSE, aView.Data()));
  VRB_CHECK(glUniformMatrix4fv(m.uModel, 1, GL_FALSE, aModel.Data()));
  return true;
}

RenderState::RenderState(State& aState, ContextWeak& aContext) : ResourceGL(aState, aContext), m(aState) {}
RenderState::~RenderState() {}

void
RenderState::InitializeGL() {
  m.vertexShader = LoadShader(GL_VERTEX_SHADER, vertexShaderSource);
  m.fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
  if (m.fragmentShader && m.vertexShader) {
    m.program = glCreateProgram();
    VRB_CHECK(glAttachShader(m.program, m.vertexShader));
    VRB_CHECK(glAttachShader(m.program, m.fragmentShader));
    VRB_CHECK(glLinkProgram(m.program));
    GLint linked = 0;
    VRB_CHECK(glGetProgramiv(m.program, GL_LINK_STATUS, &linked));
    if (!linked) {
      GLint length = 0;
      VRB_CHECK(glGetProgramiv(m.program, GL_INFO_LOG_LENGTH, &length));
      if (length > 1) {
        std::unique_ptr<char[]> log = std::make_unique<char[]>(length);
        VRB_CHECK(glGetProgramInfoLog(m.program, length, nullptr, log.get()));
        VRB_LOG("Failed to link program:\n%s", log.get());
      }
      VRB_CHECK(glDeleteProgram(m.program));
      m.program = 0;
    }
  }
  if (m.program) {
    m.uPerspective = getUniformLocation(m.program, "u_perspective");
    m.uView = getUniformLocation(m.program, "u_view");
    m.uModel = getUniformLocation(m.program, "u_model");
    m.uLightCount = getUniformLocation(m.program, "u_lightCount");
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
      m.uLights[ix].direction = getUniformLocation(m.program, direction);
      m.uLights[ix].ambient = getUniformLocation(m.program, ambient);
      m.uLights[ix].diffuse = getUniformLocation(m.program, diffuse);
      m.uLights[ix].specular = getUniformLocation(m.program, specular);
    }
    const std::string materialName("u_material.");
    const std::string specularExponentName("specularExponent");
    const std::string ambient = materialName + ambientName;
    const std::string diffuse = materialName + diffuseName;
    const std::string specular = materialName + specularName;
    const std::string specularExponent = materialName + specularExponentName;
    m.uMatterialAmbient = getUniformLocation(m.program, ambient);
    m.uMatterialDiffuse = getUniformLocation(m.program, diffuse);
    m.uMatterialSpecular = getUniformLocation(m.program, specular);
    m.uMatterialSpecularExponent = getUniformLocation(m.program, specularExponent);
    const char* positionName = "a_position";
    m.aPosition = VRB_CHECK(glGetAttribLocation(m.program, positionName));
    if (m.aPosition < 0) {
      VRB_LOG("Unable to glGetAttribLocation for '%s'", positionName);
    }
    const char* normalName = "a_normal";
    m.aNormal = VRB_CHECK(glGetAttribLocation(m.program, normalName));
    if (m.aNormal < 0) {
      VRB_LOG("Unable to glGetAttribLocation for '%s'", normalName);
    }
    m.updateMaterial = true;
  }
}

void
RenderState::ShutdownGL() {

}

GLuint
RenderState::LoadShader(GLenum type, const char* src) {
  GLuint result = 0;

  result = glCreateShader(type);

  if (result == 0) {
    VRB_LOG("FAILDED to create shader of type: %s", (type == GL_VERTEX_SHADER ? "vertex shader" : "fragment shader"));
  }

  VRB_CHECK(glShaderSource(result, 1, &src, nullptr));
  VRB_CHECK(glCompileShader(result));
  GLint compiled = 0;
  VRB_CHECK(glGetShaderiv(result, GL_COMPILE_STATUS, &compiled));

  if (!compiled) {
    GLint length = 0;
    glGetShaderiv(result, GL_INFO_LOG_LENGTH, &length);
    if (length > 1) {
      std::unique_ptr<char[]> log = std::make_unique<char[]>(length);
      VRB_CHECK(glGetShaderInfoLog(result, length, nullptr, log.get()));
      VRB_LOG("FAILED TO COMPILE SHADER:\n%s", log.get());
      VRB_LOG("From source:\n%s", src);
    }
  }

  return result;
}

} // namespace vrb
