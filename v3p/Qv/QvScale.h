#ifndef  _QV_SCALE_
#define  _QV_SCALE_

#include "QvSFVec3f.h"
#include "QvSubNode.h"

class QvScale : public QvNode
{
  QV_NODE_HEADER(QvScale);

public:
  // Fields
  QvSFVec3f scaleFactor; // Scale factors in x, y, and z

  // mpichler, 19950503
  const float* scale_;
  // mpichler, 19951001
  float invscale_ [3];
};

#endif /* _QV_SCALE_ */
