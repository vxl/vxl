// This is core/vcsl/vcsl_spherical.cxx
#include "vcsl_spherical.h"

#include <vcsl/vcsl_axis.h>
#include <vcsl/vcsl_angle.h>

//---------------------------------------------------------------------------
// Default constructor.
//---------------------------------------------------------------------------
vcsl_spherical::vcsl_spherical(void)
{
  vcsl_axis_sptr a;
  a=new vcsl_axis;
  axes_.push_back(a);
  a=new vcsl_axis(*(vcsl_angle::instance().ptr()));
  axes_.push_back(a);
  a=new vcsl_axis(*(vcsl_angle::instance().ptr()));
  axes_.push_back(a);
}
