#ifndef  _QV_NORMAL_
#define  _QV_NORMAL_

#include "QvMFVec3f.h"
#include "QvSubNode.h"

class QvNormal : public QvNode
{
  QV_NODE_HEADER(QvNormal);

public:
  // Fields
  QvMFVec3f vector; // Normal vector(s)
};

#endif /* _QV_NORMAL_ */
