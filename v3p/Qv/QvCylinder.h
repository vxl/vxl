#ifndef  _QV_CYLINDER_
#define  _QV_CYLINDER_

#include "QvSFBitMask.h"
#include "QvSFFloat.h"
#include "QvSubNode.h"

class QvCylinder : public QvNode
{
  QV_NODE_HEADER(QvCylinder);

public:

  enum Part {                   // Cylinder parts
    SIDES       = 0x01,         // The tubular part
    TOP         = 0x02,         // The top circular face
    BOTTOM      = 0x04,         // The bottom circular face
    ALL         = 0x07          // All parts
  };

  // Fields
  QvSFBitMask   parts;          // Visible parts of cylinder
  QvSFFloat     radius;         // Radius in x and z dimensions
  QvSFFloat     height;         // Size in y dimension

  // mpichler, 19950522
  int parts_;                   // parts in terms of ge3d
};

#endif /* _QV_CYLINDER_ */
