#ifndef  _QV_SPHERE_
#define  _QV_SPHERE_

#include "QvSFFloat.h"
#include "QvSubNode.h"

class QvSphere : public QvNode
{
  QV_NODE_HEADER(QvSphere);

public:
  // Fields
  QvSFFloat radius; // Radius of sphere
};

#endif /* _QV_SPHERE_ */

