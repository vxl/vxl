#ifndef  _QV_MF_COLOR_
#define  _QV_MF_COLOR_

#include "QvSubField.h"

class QvMFColor : public QvMField {
  public:
    float *values; // 3 per color
    QV_MFIELD_HEADER(QvMFColor);
};

#endif /* _QV_MF_COLOR_ */
