/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_PROGRAM_DOT_H
#define VRB_PROGRAM_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/gl.h"

#include <string>

namespace vrb {

class Program {
public:
  static ProgramPtr Create();
  bool Enable();
  void SetFeatures(const uint32_t aFeatures);
  bool SupportsFeatures(const uint32_t aFeatures);
  void SetProgram(GLuint aProgram);
  GLuint GetProgram() const;
  GLint GetAttributeLocation(const char* aName);
  GLint GetAttributeLocation(const std::string &aName) { return GetAttributeLocation(aName.c_str()); }
  GLint GetUniformLocation(const char* aName);
  GLint GetUniformLocation(const std::string &aName) { return GetUniformLocation(aName.c_str()); }
protected:
  struct State;
  Program(State& aState);
  ~Program() = default;
private:
  State& m;
  Program() = delete;
  VRB_NO_DEFAULTS(Program)
};

} // namespace vrb

#endif // VRB_PROGRAM_DOT_H
