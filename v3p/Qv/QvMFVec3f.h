#ifndef  _QV_MF_VEC3F_
#define  _QV_MF_VEC3F_

#include "QvSubField.h"

class QvMFVec3f : public QvMField {
  public:
    float *values;
    QV_MFIELD_HEADER(QvMFVec3f);
};

#endif /* _QV_MF_VEC3F_ */
