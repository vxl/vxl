// This is core/vcsl/vcsl_cartesian_2d.cxx
#include "vcsl_cartesian_2d.h"

#include <vcsl/vcsl_axis.h>

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_cartesian_2d::vcsl_cartesian_2d(void)
{
  vcsl_axis_sptr a;
  a=new vcsl_axis;
  axes_.push_back(a);
  a=new vcsl_axis(*(a.ptr()));
  axes_.push_back(a);
  right_handed_=true;
}
