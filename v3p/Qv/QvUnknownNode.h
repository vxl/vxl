#ifndef  _QV_UNKNOWN_NODE_
#define  _QV_UNKNOWN_NODE_

#include "QvGroup.h"

class QvUnknownNode : public QvGroup {

    QV_NODE_HEADER(QvUnknownNode);

  public:
    const char  *className;
    QvFieldData *instanceFieldData;

    void setClassName(const char *name);
};

#endif /* _QV_UNKNOWN_NODE_ */

