#ifndef  _QV_SF_BOOL_
#define  _QV_SF_BOOL_

#include "QvSubField.h"

class QvSFBool : public QvSField {
  public:
    QvBool value;
    QV_SFIELD_HEADER(QvSFBool);
};

#endif /* _QV_SF_BOOL_ */
