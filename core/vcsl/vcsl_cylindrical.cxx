// This is core/vcsl/vcsl_cylindrical.cxx
#include "vcsl_cylindrical.h"

#include <vcsl/vcsl_axis.h>
#include <vcsl/vcsl_angle.h>

//---------------------------------------------------------------------------
// Default constructor.
//---------------------------------------------------------------------------
vcsl_cylindrical::vcsl_cylindrical(void)
{
  vcsl_axis_sptr a;
  a=new vcsl_axis;
  axes_.push_back(a);
  a=new vcsl_axis(*(vcsl_angle::instance().ptr()));
  axes_.push_back(a);
  a=new vcsl_axis;
  axes_.push_back(a);
}
