#ifndef  _QV_SF_STRING_
#define  _QV_SF_STRING_

#include "QvSubField.h"

class QvSFString : public QvSField {
  public:
    QvString value;
    QV_SFIELD_HEADER(QvSFString);
};

#endif /* _QV_SF_STRING_ */
