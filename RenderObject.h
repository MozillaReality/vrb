#ifndef VRB_RENDER_OBJECT_DOT_H
#define VRB_RENDER_OBJECT_DOT_H

#include "vrb/Vector.h"
#include <string>
#include <vector>

namespace vrb {

class RenderObject {
public:
  RenderObject();
  RenderObject(const std::string& aName);
  ~RenderObject();
  std::string GetName() const;
  void Draw();

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
  struct State;
  State &m;
};

}

#endif //  VRB_RENDER_OBJECT_DOT_H
