#ifndef  _QV_COORDINATE3_
#define  _QV_COORDINATE3_

#include "QvMFVec3f.h"
#include "QvSubNode.h"

#include "vectors.h"

class QvCoordinate3 : public QvNode
{
  QV_NODE_HEADER(QvCoordinate3);

public:
  // Fields
  QvMFVec3f point; // Coordinate point(s)

  // point3D min_, max_;  // mpichler, 19950510
  // mpichler, 19951006: use object boundings of QvNode

  int minmaxbuilt_;  // mpichler, 19960614
};

#endif /* _QV_COORDINATE3_ */
