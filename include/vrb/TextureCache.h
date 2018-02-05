/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_TEXTURE_CACHE_DOT_H
#define VRB_TEXTURE_CACHE_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

#include <string>

namespace vrb {

class TextureCache {
public:
  static TextureCachePtr Create(ContextWeak& aContext);
  void Init();
  void Shutdown();
  TexturePtr LoadTexture(const std::string& aFileName);
  TexturePtr GetDefaultTexture();
protected:
  struct State;
  TextureCache(State& aState, ContextWeak& aContext);
  ~TextureCache();
private:
  State &m;
  TextureCache() = delete;
  VRB_NO_DEFAULTS(TextureCache)
};

}

#endif // VRB_TEXTURE_CACHE_DOT_H
