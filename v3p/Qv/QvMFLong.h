#ifndef  _QV_MF_LONG_
#define  _QV_MF_LONG_

#include "QvSubField.h"
#include <vxl_config.h> // for vxl_int_32

class QvMFLong : public QvMField
{
 public:
  // different sizes of int (4) and long (8) on DEC ALPHA, prefer 4-byte integer
  vxl_int_32* values;  // mpichler, 19950428
  QV_MFIELD_HEADER(QvMFLong);
};

#endif // _QV_MF_LONG_
