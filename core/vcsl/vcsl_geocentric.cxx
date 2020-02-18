// This is core/vcsl/vcsl_geocentric.cxx
#include "vcsl_geocentric.h"
#include "vcsl/vcsl_axis.h"
#include "vcsl/vcsl_angle.h"

//---------------------------------------------------------------------------
// Default constructor.
//---------------------------------------------------------------------------
#if 0 // Private constructor can never be used
vcsl_geocentric::vcsl_geocentric()
{
  vcsl_axis_sptr a = new vcsl_axis;
  axes_.push_back(a);
  a = new vcsl_axis(vcsl_angle::instance().ptr());
  axes_.push_back(a);
  a = new vcsl_axis(vcsl_angle::instance().ptr());
  axes_.push_back(a);
}
#endif
