#ifndef  _QV_CUBE_
#define  _QV_CUBE_

#include "QvSFFloat.h"
#include "QvSubNode.h"

class QvCube : public QvNode
{
  QV_NODE_HEADER(QvCube);

public:
  // Fields
  QvSFFloat   width;  // Size in x dimension
  QvSFFloat   height; // Size in y dimension
  QvSFFloat   depth;  // Size in z dimension

  // point3D min_, max_;  // mpichler, 19950502
  // mpichler, 19951006: use object boundings of QvNode
};

#endif /* _QV_CUBE_ */
