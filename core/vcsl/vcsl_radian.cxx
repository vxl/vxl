// This is core/vcsl/vcsl_radian.cxx
#include "vcsl_radian.h"

//---------------------------------------------------------------------------
// Return the reference to the unique vcsl_radian object
//---------------------------------------------------------------------------
vcsl_radian_sptr vcsl_radian::instance()
{
  static vcsl_radian_sptr instance_=new vcsl_radian;
  return instance_;
}
