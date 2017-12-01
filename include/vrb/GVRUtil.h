#ifndef VRB_GVR_UTIL_DOT_H
#define VRB_GVR_UTIL_DOT_H

#include "vr/gvr/capi/include/gvr_types.h"
#include "vrb/Matrix.h"
#include "vrb/Vector.h"

namespace vrb {

// gvr is row major, dmz is column major so transpose.
inline Matrix convert(gvr::Mat4f& aMatrix) {
  return Matrix(
    aMatrix[0][0], aMatrix[1][0], aMatrix[2][0], aMatrix[3][0],
    aMatrix[0][1], aMatrix[1][1], aMatrix[2][1], aMatrix[3][1],
    aMatrix[0][2], aMatrix[1][2], aMatrix[2][2], aMatrix[3][2],
    aMatrix[0][3], aMatrix[1][3], aMatrix[2][3], aMatrix[3][3]);
}

}

#endif //  VRB_GVR_UTIL_DOT_H
