/* -*- Mode: C++; tab-width: 20; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef VRB_DRAWABLE_LIST_STATE_DOT_H
#define VRB_DRAWABLE_LIST_STATE_DOT_H

#include "vrb/DrawableList.h"
#include "vrb/Color.h"
#include "vrb/Light.h"
#include "vrb/Matrix.h"

namespace vrb {

struct DrawableList::State {
  struct LightSnapshot {
    LightSnapshot* next;
    LightSnapshot* masterNext;
    const uint32_t id;
    const int depth;
    const Vector direction;
    const Color ambient;
    const Color diffuse;
    const Color specular;
    LightSnapshot(const uint32_t aId, const int aDepth, const Light& aLight)
      : next(nullptr)
      , masterNext(nullptr)
      , id(aId)
      , depth(aDepth)
      , direction(aLight.GetDirection())
      , ambient(aLight.GetAmbientColor())
      , diffuse(aLight.GetDiffuseColor())
      , specular(aLight.GetSpecularColor()) {}
    ~LightSnapshot() {}
  };
  struct LambdaSnapshot {
    LambdaSnapshot(const Lambda& aPreRender, const Lambda& aPostRender)
      : next(nullptr)
      , preRender(aPreRender)
      , postRender(aPostRender) {}
    LambdaSnapshot* next;
    Lambda preRender;
    Lambda postRender;
  };
  struct DrawNode {
    DrawNode* next;
    LightSnapshot* lights;
    LambdaSnapshot* lambdas;
    DrawablePtr drawable;
    Matrix transform;
    DrawNode() : next(nullptr), lights(nullptr), lambdas(nullptr) {}
  };

  DrawNode* drawables;
  LightSnapshot* currentLights;
  LightSnapshot* lights;
  LambdaSnapshot* lambdas;
  LambdaSnapshot* currentLambdas;
  uint32_t idCount;
  int depth;

  State() : drawables(nullptr), currentLights(nullptr), lights(nullptr), lambdas(nullptr), currentLambdas(nullptr), idCount(0), depth(0) {}
  ~State() { Reset(); }
  void Reset();
};

}

#endif // VRB_DRAWABLE_LIST_STATE_DOT_H
