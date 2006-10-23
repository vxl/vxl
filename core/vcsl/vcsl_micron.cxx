// This is core/vcsl/vcsl_micron.cxx
#include "vcsl_micron.h"

//---------------------------------------------------------------------------
// Return the reference to the unique vcsl_micron object
//---------------------------------------------------------------------------
vcsl_micron_sptr vcsl_micron::instance()
{
  static vcsl_micron_sptr instance_=new vcsl_micron;
  return instance_;
}
