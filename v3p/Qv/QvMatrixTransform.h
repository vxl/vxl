#ifndef  _QV_MATRIX_TRANSFORM_
#define  _QV_MATRIX_TRANSFORM_

#include "QvSFMatrix.h"
#include "QvSubNode.h"

class QvMatrixTransform : public QvNode
{
  QV_NODE_HEADER(QvMatrixTransform);

public:
  // Fields
  QvSFMatrix       matrix; // Transformation matrix

  // mpichler, 19950509
  const float (* mat_)[4]; // pointer to matrix
  // mpichler, 19951004
  matrix4D invmat_, invtranspmat3D_;
};

#endif /* _QV_MATRIX_TRANSFORM_ */
