#ifndef  _QV_LISTS_
#define  _QV_LISTS_

#include "QvPList.h"

class QvField;
class QvNode;

class QvNodeList : public QvPList {
  public:
    QvNodeList();
    ~QvNodeList() { truncate(0); }
    void      append(QvNode *node);
    void      insert (int where, QvNode* node);  // anuss: insert node before
    void      remove(int which);
    void      truncate(int start);
    QvNode *  operator [](int i) const
        { return ( (QvNode *) ( (* (const QvPList *) this) [i]) ); }
};

#endif /* _QV_LISTS_ */
