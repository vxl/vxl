#ifndef  _QV_LEVEL_OF_DETAIL_
#define  _QV_LEVEL_OF_DETAIL_

#include "QvMFFloat.h"
#include "QvGroup.h"
#include "QvSFVec3f.h"

#include "vectors.h"

class QvLOD : public QvGroup
{
  QV_NODE_HEADER(QvLOD);

public:
  // Fields
  QvMFFloat    range;     // Distance ranges for LOD switching
  QvSFVec3f    center;    // Center for distance computation

  // mpichler, 19950724
  const point3D* center_; // center for LOD computation
  // mpichler, 19951010
  int lastdrawn_;         // most recently drawn child
};

#endif /* _QV_LEVEL_OF_DETAIL_ */
