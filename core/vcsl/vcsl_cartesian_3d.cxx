// This is core/vcsl/vcsl_cartesian_3d.cxx
#include "vcsl_cartesian_3d.h"

#include <vcsl/vcsl_axis.h>

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_cartesian_3d::vcsl_cartesian_3d(void)
{
  vcsl_axis_sptr a;
  a=new vcsl_axis;
  axes_.push_back(a);
  a=new vcsl_axis(*(a.ptr()));
  axes_.push_back(a);
  a=new vcsl_axis(*(a.ptr()));
  axes_.push_back(a);
  right_handed_=true;
}
