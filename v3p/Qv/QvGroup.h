#ifndef  _QV_GROUP_
#define  _QV_GROUP_

class QvChildList;
#include "QvSubNode.h"

class QvGroup : public QvNode {

    QV_NODE_HEADER(QvGroup);

  public:
    QvNode *            getChild(int index) const;
    int                 getNumChildren() const;
    virtual QvChildList *getChildren() const;
    virtual QvBool      readInstance(QvInput *in);
    virtual QvBool      readChildren(QvInput *in);
};

#endif /* _QV_GROUP_ */
