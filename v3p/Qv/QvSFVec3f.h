#ifndef  _QV_SF_VEC3F_
#define  _QV_SF_VEC3F_

#include "QvSubField.h"

class QvSFVec3f : public QvSField {
  public:
    float value[3];
    QV_SFIELD_HEADER(QvSFVec3f);
};

#endif /* _QV_SF_VEC3F_ */
