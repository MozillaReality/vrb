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
    const int depth;
    const Vector direction;
    const Color ambient;
    const Color diffuse;
    const Color specular;
    LightSnapshot(const int aDepth, const Light& aLight)
      : next(nullptr)
      , masterNext(nullptr)
      , depth(aDepth)
      , direction(aLight.GetDirection())
      , ambient(aLight.GetAmbientColor())
      , diffuse(aLight.GetDiffuseColor())
      , specular(aLight.GetSpecularColor()) {}
    ~LightSnapshot() {}
  };
  struct DrawNode {
    DrawNode* next;
    LightSnapshot* lights;
    DrawablePtr drawable;
    Matrix transform;

    DrawNode() : next(nullptr), lights(nullptr) {}
  };

  DrawNode* drawables;
  LightSnapshot* currentLights;
  LightSnapshot* lights;
  int depth;

  State() : drawables(nullptr), currentLights(nullptr), lights(nullptr), depth(0) {}
  ~State() { Reset(); }
  void Reset();
};

}

#endif // VRB_DRAWABLE_LIST_STATE_DOT_H
