#ifndef  _QV_SF_ROTATION_
#define  _QV_SF_ROTATION_

#include "QvSubField.h"

class QvSFRotation : public QvSField {
  public:
    float axis[3];
    float angle;
    QV_SFIELD_HEADER(QvSFRotation);
};

#endif /* _QV_SF_ROTATION_ */
