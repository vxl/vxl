#ifndef  _QV_MF_FLOAT_
#define  _QV_MF_FLOAT_

#include "QvSubField.h"

class QvMFFloat : public QvMField {
  public:
    float *values;
    QV_MFIELD_HEADER(QvMFFloat);
};

#endif /* _QV_MF_FLOAT_ */
