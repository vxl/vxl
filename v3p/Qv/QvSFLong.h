#ifndef  _QV_SF_LONG_
#define  _QV_SF_LONG_

#include "QvSubField.h"

class QvSFLong : public QvSField {
  public:
    long value;
    QV_SFIELD_HEADER(QvSFLong);
};

#endif /* _QV_SF_LONG_ */
