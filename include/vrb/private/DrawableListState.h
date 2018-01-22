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
  uint32_t idCount;
  int depth;

  State() : drawables(nullptr), currentLights(nullptr), lights(nullptr), idCount(0), depth(0) {}
  ~State() { Reset(); }
  void Reset();
};

}

#endif // VRB_DRAWABLE_LIST_STATE_DOT_H
