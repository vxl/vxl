#ifndef  _QV_SF_FLOAT_
#define  _QV_SF_FLOAT_

#include "QvSubField.h"

class QvSFFloat : public QvSField {
  public:
    float value;
    QV_SFIELD_HEADER(QvSFFloat);
};

#endif /* _QV_SF_FLOAT_ */
