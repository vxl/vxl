#ifndef  _QV_ROTATION_
#define  _QV_ROTATION_

#include "QvSFRotation.h"
#include "QvSubNode.h"

#include "vectors.h"

class QvRotation : public QvNode
{
  QV_NODE_HEADER(QvRotation);

public:
  // Fields
  QvSFRotation  rotation; // Rotation

  // mpichler, 19950509
// const vector3D* axis_; // rotation axis
// float angle_;          // and angle (rad)

  // mpichler, 19951004
  matrix4D mat_;          // transformation matrix
  matrix4D invmat_;       // and its inverse
};

#endif /* _QV_ROTATION_ */
