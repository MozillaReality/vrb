#ifndef VRB_RENDER_OBJECT_DOT_H
#define VRB_RENDER_OBJECT_DOT_H

#include "vrb/Forward.h"
#include <string>
#include <vector>

namespace vrb {

class RenderObject {
public:
  static RenderObjectPtr Create(const std::string &Name);

  std::string GetName() const;
  void Init();
  void Draw(const Matrix& aProjection);
  void SetTransform(const Matrix& aTransform);

  int AddVertex(const Vector& aVertex);
  int AddNormal(const Vector& aNormal);
  int AddUV(const Vector& aUV);
  int CreateGroup(const std::vector<std::string>& aNames);
  int AddFace(
      const int aGroup,
      const std::vector<int>& aVerticies,
      const std::vector<int>& aUVs,
      const std::vector<int>& aNormals);

  void Dump();

protected:
  RenderObject();
  ~RenderObject();

  struct State;
  State *m;
};

}

#endif //  VRB_RENDER_OBJECT_DOT_H
