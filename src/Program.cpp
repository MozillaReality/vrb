/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include <vrb/ShaderUtil.h>
#include <vrb/GLError.h>
#include "vrb/Program.h"

#include "vrb/ConcreteClass.h"

namespace vrb {

struct Program::State {
  GLuint program = 0;
  uint32_t features = 0;
};

ProgramPtr
Program::Create() {
  return std::make_shared<ConcreteClass<Program, Program::State> >();
}

bool
Program::Enable() {
  if (!m.program) {
    return false;
  }

  VRB_GL_CHECK(glUseProgram(m.program));
  return true;
}

void
Program::SetFeatures(const uint32_t aFeatures) {
  m.features = aFeatures;
}

bool
Program::SupportsFeatures(const uint32_t aFeatures) {
  return (m.features & aFeatures) == aFeatures;
}

void
Program::SetProgram(GLuint aProgram) {
  m.program = aProgram;
}

GLuint
Program::GetProgram() const {
  return m.program;
}

GLint
Program::GetAttributeLocation(const char* aName) {
  if (!m.program) {
    return -1;
  }

  return vrb::GetAttributeLocation(m.program, aName);
}

GLint
Program::GetUniformLocation(const char* aName) {
  if (!m.program) {
    return -1;
  }

  return vrb::GetUniformLocation(m.program, aName);
}


Program::Program(State& aState) : m(aState) {}

} // namespace vrb

