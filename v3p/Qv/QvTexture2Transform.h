#ifndef  _QV_TEXTURE_2_TRANSFORM_
#define  _QV_TEXTURE_2_TRANSFORM_

#include "QvSFFloat.h"
#include "QvSFVec2f.h"
#include "QvSubNode.h"

class QvTexture2Transform : public QvNode
{
  QV_NODE_HEADER(QvTexture2Transform);

public:
  // Fields
  QvSFVec2f  translation; // Translation vector
  QvSFFloat  rotation;    // Rotation
  QvSFVec2f  scaleFactor; // Scale factors
  QvSFVec2f  center;      // Center point for scale and rotate

  matrix4D mat_;          // mpichler, 19960216
};

#endif /* _QV_TEXTURE_2_TRANSFORM_ */
