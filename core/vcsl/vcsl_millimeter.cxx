// This is core/vcsl/vcsl_millimeter.cxx
#include "vcsl_millimeter.h"

//---------------------------------------------------------------------------
// Return the reference to the unique vcsl_millimeter object
//---------------------------------------------------------------------------
vcsl_millimeter_sptr vcsl_millimeter::instance()
{
  static vcsl_millimeter_sptr instance_=new vcsl_millimeter;
  return instance_;
}
