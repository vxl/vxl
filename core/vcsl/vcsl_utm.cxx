// This is core/vcsl/vcsl_utm.cxx
#include "vcsl_utm.h"
#include <vcsl/vcsl_axis.h>

//--------------------------------------------------------------------------
// Default constructor
//--------------------------------------------------------------------------
vcsl_utm::vcsl_utm()
{
  vcsl_axis_sptr a=new vcsl_axis;
  axes_.push_back(a);
  a=new vcsl_axis;
  axes_.push_back(a);
  a=new vcsl_axis;
  axes_.push_back(a);
}
