#ifndef  _QV_SF_COLOR_
#define  _QV_SF_COLOR_

#include "QvSubField.h"

class QvSFColor : public QvSField {
  public:
    float value[3];
    QV_SFIELD_HEADER(QvSFColor);
};

#endif /* _QV_SF_COLOR_ */
