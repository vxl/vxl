// This is core/vcsl/vcsl_lambertian.cxx
#include "vcsl_lambertian.h"
#include <vcsl/vcsl_axis.h>

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_lambertian::vcsl_lambertian()
{
  vcsl_axis_sptr a=new vcsl_axis;
  axes_.push_back(a);
  a=new vcsl_axis(*a);
  axes_.push_back(a);
  a=new vcsl_axis(*a);
  axes_.push_back(a);
}
