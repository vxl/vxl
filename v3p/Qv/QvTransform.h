#ifndef  _QV_TRANSFORM_
#define  _QV_TRANSFORM_

#include "QvSFRotation.h"
#include "QvSFVec3f.h"
#include "QvSubNode.h"

#include "vectors.h"

class QvTransform : public QvNode
{
  QV_NODE_HEADER(QvTransform);

public:
  // Fields
  QvSFVec3f     translation;      // Translation vector
  QvSFRotation  rotation;         // Rotation
  QvSFVec3f     scaleFactor;      // Scale factors
  QvSFRotation  scaleOrientation; // Defines rotational space for scale
  QvSFVec3f     center;           // Center point for scale and rotate

  // mpichler, 19950516
  matrix4D mat_;
  // mpichler, 19951003
  matrix4D invmat_, invtranspmat3D_;
};

#endif /* _QV_TRANSFORM_ */
