#ifndef VRB_GEOMETRY_H
#define VRB_GEOMETRY_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/Drawable.h"
#include "vrb/Node.h"
#include "vrb/ResourceGL.h"

#include <vector>

namespace vrb {

class Geometry : public Node, protected ResourceGL, public Drawable {
public:
  static GeometryPtr Create(ContextWeak& aContext);

  // Node interface
  void Cull(CullVisitorPtr& aVisitor, DrawableListPtr& aDrawables) override;

  // Geometry interface
  RenderStatePtr GetRenderState() const;
  void SetRenderState(const RenderStatePtr& aRenderState);

  VertexArrayPtr GetVertexArray() const;
  void SetVertexArray(const VertexArrayPtr& aVertexArray);

  void AddFace(
    const std::vector<int> &aVerticies,
    const std::vector<int> &aUVs,
    const std::vector<int> &aNormals);

protected:
  struct State;
  Geometry(State& aState, ContextWeak& aContext);
  ~Geometry();

  // From ResourceGL
  void InitializeGL() override;
  void ShutdownGL() override;

  // From Drawable
  void Draw(const Camera& aCamera, const Matrix& aModelTransform) override;

private:
  State& m;
  Geometry() = delete;
  VRB_NO_DEFAULTS(Geometry)
};

}

#endif //  VRB_GEOMETRY_H
