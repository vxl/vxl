#ifndef  _QV_SF_MATRIX_
#define  _QV_SF_MATRIX_

#include "QvSubField.h"

class QvSFMatrix : public QvSField {
  public:
    float value[4][4];
    QV_SFIELD_HEADER(QvSFMatrix);
};

#endif /* _QV_SF_MATRIX_ */
