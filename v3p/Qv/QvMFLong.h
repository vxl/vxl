#ifndef  _QV_MF_LONG_
#define  _QV_MF_LONG_

#include "QvSubField.h"

class QvMFLong : public QvMField {
 public:
#ifdef __osf__
  // different sizes of int (4) and long (8) on DEC ALPHA, prefer 4-byte integer
  int* values;  // mpichler, 19950428
#else
  long *values;
#endif
  QV_MFIELD_HEADER(QvMFLong);
};

#endif /* _QV_MF_LONG_ */
