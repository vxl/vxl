#ifndef  _QV_INFO_
#define  _QV_INFO_

#include "QvSFString.h"
#include "QvSubNode.h"

class QvInfo : public QvNode
{
  QV_NODE_HEADER(QvInfo);

public:
  // Fields
  QvSFString string; // Info string

  // mpichler, 19951116
  int handled_;  // viewer info already handled?
};

#endif /* _QV_INFO_ */
