#include "vrb/RenderObjectState.h"

#include "vrb/Logger.h"
#include "vrb/GLError.h"
#include "vrb/Matrix.h"

#include <GLES2/gl2.h>
#include <string>

namespace {

static const char* vertexShaderSource = R"SHADER(

uniform mat4 u_matViewProjection;

attribute vec4 a_position;

void main(void) {
  gl_Position = u_matViewProjection * a_position;
}

)SHADER";

static const char* fragmentShaderSource = R"SHADER(

void main() {
  gl_FragColor = vec4(1, 1, 1, 1);
}

)SHADER";

class RenderObjectStateAlloc : public vrb::RenderObjectState {
};

}

namespace vrb {

struct RenderObjectState::State {
  GLuint vertexShader;
  GLuint fragmentShader;
  GLuint program;
  GLint uniformProjectionMatrix;
  GLint attributePosition;

  State()
      : vertexShader(0)
      , fragmentShader(0)
      , program(0)
      , uniformProjectionMatrix(0)
      , attributePosition(0)
  {}
};

RenderObjectStatePtr
RenderObjectState::Create() {
  return std::make_shared<RenderObjectStateAlloc>();
}

GLuint
RenderObjectState::Program() const {
  return m.program;
}

GLint
RenderObjectState::AttributePosition() const {
  return m.attributePosition;
}

void
RenderObjectState::Init() {
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
        char* log = new char[length];
        VRB_LOG("Failed to link program:\n%s", log);
        delete[] log;
      }
      VRB_CHECK(glDeleteProgram(m.program));
      m.program = 0;
    }
  }
  if (m.program) {
    const char* varName = "u_matViewProjection";
    m.uniformProjectionMatrix = glGetUniformLocation(m.program, varName);
    if (m.uniformProjectionMatrix < 0) {
      VRB_LOG("Unable to glGetUniformLocation for '%s'", varName);
    }
    m.attributePosition = glGetAttribLocation(m.program, "a_position");
  }
}

bool
RenderObjectState::Enable(const Matrix& aProjection) {
  if (!m.program) { return false; }
  VRB_CHECK(glUseProgram(m.program));
  VRB_CHECK(glUniformMatrix4fv(m.uniformProjectionMatrix, 1, GL_FALSE, aProjection.Data()));
  return true;
}

RenderObjectState::RenderObjectState() : m(*(new State)) {

}

RenderObjectState::~RenderObjectState() {
  delete &m;
}

GLuint
RenderObjectState::LoadShader(GLenum type, const char* src) {
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
      char* log = new char[length];
      VRB_CHECK(glGetShaderInfoLog(result, length, nullptr, log));
      VRB_LOG("FAILED TO COMPILE SHADER:\n%s", log);
      VRB_LOG("From source:\n%s", src);
      delete[] log;
    }
  }

  return result;
}

} // namespace vrb
