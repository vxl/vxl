#ifndef  _QV_MF_VEC2F_
#define  _QV_MF_VEC2F_

#include "QvSubField.h"

class QvMFVec2f : public QvMField {
  public:
    float *values;
    QV_MFIELD_HEADER(QvMFVec2f);
};

#endif /* _QV_MF_VEC2F_ */
