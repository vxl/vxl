// This is core/vcsl/vcsl_meter.cxx
#include "vcsl_meter.h"

//---------------------------------------------------------------------------
// Return the reference to the unique vcsl_meter object
//---------------------------------------------------------------------------
vcsl_meter_sptr vcsl_meter::instance()
{
  static vcsl_meter_sptr instance_=new vcsl_meter;
  return instance_;
}
