#include "vrb/RenderState.h"
#include "vrb/private/ResourceGLState.h"

#include "vrb/ConcreteClass.h"
#include "vrb/Logger.h"
#include "vrb/GLError.h"
#include "vrb/Matrix.h"
#include "vrb/Vector.h"

#include <GLES2/gl2.h>
#include <string>

namespace {

static const char* vertexShaderSource = R"SHADER(

uniform mat4 u_perspective;
uniform mat4 u_view;
uniform mat4 u_model;

attribute vec4 a_position;
attribute vec4 a_normal;

void main(void) {
  gl_Position = u_perspective * u_view * u_model * a_position;
}

)SHADER";

static const char* fragmentShaderSource = R"SHADER(

void main() {
  gl_FragColor = vec4(1, 1, 1, 1);
}

)SHADER";

}

namespace vrb {

struct RenderState::State : public ResourceGL::State {
  GLuint vertexShader;
  GLuint fragmentShader;
  GLuint program;
  GLint uPerspective;
  GLint uView;
  GLint uModel;
  GLint aPosition;
  GLint aNormal;

  State()
      : vertexShader(0)
      , fragmentShader(0)
      , program(0)
      , uPerspective(0)
      , uView(0)
      , uModel(0)
      , aPosition(0)
      , aNormal(0)
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

bool
RenderState::Enable(const Matrix& aPerspective, const Matrix& aView, const Matrix& aModel) {
  if (!m.program) { return false; }
  VRB_CHECK(glUseProgram(m.program));
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
    const char* perspectiveName = "u_perspective";
    m.uPerspective = glGetUniformLocation(m.program, perspectiveName);
    if (m.uPerspective < 0) {
      VRB_LOG("Unable to glGetUniformLocation for '%s'", perspectiveName);
    }
    const char* viewName = "u_view";
    m.uView = glGetUniformLocation(m.program, viewName);
    const char* modelName = "u_model";
    m.uModel = glGetUniformLocation(m.program, modelName);
    m.aPosition = glGetAttribLocation(m.program, "a_position");
    m.aNormal = glGetAttribLocation(m.program, "a_normal");
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
