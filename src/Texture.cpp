/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "vrb/Texture.h"
#include "vrb/private/TextureState.h"

#include "vrb/ConcreteClass.h"
#include "vrb/GLError.h"
#include "vrb/Logger.h"

namespace vrb {

TexturePtr
Texture::Create(ContextWeak& aContext) {
  return std::make_shared<ConcreteClass<Texture, Texture::State> >(aContext);
}

void
Texture::Bind() {
  AboutToBind();
  VRB_CHECK(glBindTexture(m.target, m.texture));
}

void
Texture::Unbind() {
  VRB_CHECK(glBindTexture(m.target, 0));
}

std::string
Texture::GetName() const {
  return m.name;
}
void
Texture::SetName(const std::string& aName) {
  m.name = aName;
}


Texture::Texture(State& aState, ContextWeak& aContext) : m(aState) {}
Texture::~Texture() {}

} // namespace vrb
