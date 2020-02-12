/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_BASIC_SHADERS_DOT_H
#define VRB_BASIC_SHADERS_DOT_H

#define VRB_MAX_LIGHTS 2

namespace vrb {

const char* GetVertexShaderSource();
const char* GetFragmentShaderSource();
const char* GetFragmentTextureShaderSource();
const char* GetFragmentSurfaceTextureShaderSource();
const char* GetFragmentCubeMapTextureShaderSource();

} // namespace vrb

#endif // VRB_BASIC_SHADERS_DOT_H
