#ifndef  _QV_SWITCH_
#define  _QV_SWITCH_

#include "QvSFLong.h"
#include "QvGroup.h"

#define QV_SWITCH_NONE (-1)  /* Don't traverse any children  */
#define QV_SWITCH_ALL  (-3)  /* Traverse all children        */

class QvSwitch : public QvGroup
{
  QV_NODE_HEADER(QvSwitch);

public:
  // Fields
  QvSFLong              whichChild;     // Child to traverse
};

#endif /* _QV_SWITCH_ */
