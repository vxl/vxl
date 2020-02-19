// This is core/vcsl/vcsl_cartesian_2d.cxx
#include "vcsl_cartesian_2d.h"
#include "vcsl/vcsl_axis.h"

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_cartesian_2d::vcsl_cartesian_2d()

{
  vcsl_axis_sptr a = new vcsl_axis;
  axes_.push_back(a);
  a = new vcsl_axis(*a);
  axes_.push_back(a);
}
