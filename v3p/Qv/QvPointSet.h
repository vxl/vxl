#ifndef  _QV_POINT_SET_
#define  _QV_POINT_SET_

#include "QvSFLong.h"
#include "QvSubNode.h"

#define QV_POINT_SET_USE_REST_OF_POINTS (-1)

class QvPointSet : public QvNode
{
  QV_NODE_HEADER(QvPointSet);

public:
  // Fields
  QvSFLong              startIndex;     // Index of 1st coordinate of shape
  QvSFLong              numPoints;      // Number of points to draw

  // mpichler, 19950512
  const point3D* points_;               // start of point array
  int num_;                             // number of points

  // mpichler, 19951019
  float epsilon_;                       // picking tolerance
};

#endif /* _QV_POINT_SET_ */
