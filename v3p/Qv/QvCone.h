#ifndef  _QV_CONE_
#define  _QV_CONE_

#include "QvSFBitMask.h"
#include "QvSFFloat.h"
#include "QvSubNode.h"

class QvCone : public QvNode
{
  QV_NODE_HEADER(QvCone);

public:

  enum Part {                   // Cone parts:
    SIDES       = 0x01,         // The conical part
    BOTTOM      = 0x02,         // The bottom circular face
    ALL         = 0x03          // All parts
  };

  // Fields
  QvSFBitMask   parts;          // Visible parts of cone
  QvSFFloat     bottomRadius;   // Radius of bottom circular face
  QvSFFloat     height;         // Size in y dimension

  // mpichler, 19950522
  int parts_;                   // parts in terms of ge3d
};

#endif /* _QV_CONE_ */
