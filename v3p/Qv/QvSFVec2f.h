#ifndef  _QV_SF_VEC2F_
#define  _QV_SF_VEC2F_

#include "QvSubField.h"

class QvSFVec2f : public QvSField {
  public:
    float value[2];
    QV_SFIELD_HEADER(QvSFVec2f);
};

#endif /* _QV_SF_VEC2F_ */
