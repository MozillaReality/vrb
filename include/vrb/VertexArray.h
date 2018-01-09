#ifndef VRB_VERTEX_ARRAY_DOT_H
#define VRB_VERTEX_ARRAY_DOT_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"

namespace vrb {

class VertexArray {
public:
  static VertexArrayPtr Create(ContextWeak& aContext);

  int GetVertexCount() const;
  int GetNormalCount() const;
  int GetUVCount() const;

  const Vector& GetVertex(const int aIndex) const;
  const Vector& GetNormal(const int aIndex) const;
  const Vector& GetUV(const int aIndex) const;

  void SetVertex(const int aIndex, const Vector& aPoint);
  void SetNormal(const int aIndex, const Vector& aNormal);
  void SetUV(const int aIndex, const Vector& aUV);

  int AddVertex(const Vector& aPoint);
  int AddNormal(const Vector& aNormal);
  int AddUV(const Vector& aUV);

protected:
  struct State;
  VertexArray(State& aState, ContextWeak& aContext);
  ~VertexArray();

private:
  State& m;
  VertexArray() = delete;
  VRB_NO_DEFAULTS(VertexArray);
};

}

#endif //  VRB_VERTEX_ARRAY_DOT_H
