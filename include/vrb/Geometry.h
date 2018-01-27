#ifndef VRB_GEOMETRY_H
#define VRB_GEOMETRY_H

#include "vrb/Forward.h"
#include "vrb/MacroUtils.h"
#include "vrb/Drawable.h"
#include "vrb/Node.h"
#include "vrb/ResourceGL.h"

#include <vector>

namespace vrb {

class Geometry : public Node, public Drawable, protected ResourceGL {
public:
  static GeometryPtr Create(ContextWeak& aContext);

  // Node interface
  void Cull(CullVisitor& aVisitor, DrawableList& aDrawables) override;

  // From Drawable
  RenderStatePtr& GetRenderState() override;
  void SetRenderState(const RenderStatePtr& aRenderState) override;
  void Draw(const Camera& aCamera, const Matrix& aModelTransform) override;

  // Geometry interface
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
  void InitializeGL(Context& aContext) override;
  void ShutdownGL(Context& aContext) override;

private:
  State& m;
  Geometry() = delete;
  VRB_NO_DEFAULTS(Geometry)
};

}

#endif //  VRB_GEOMETRY_H
