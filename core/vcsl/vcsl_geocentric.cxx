// This is vxl/vcsl/vcsl_geocentric.cxx
#include "vcsl_geocentric.h"

#include <vcsl/vcsl_axis.h>
#include <vcsl/vcsl_angle.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor.
//---------------------------------------------------------------------------
vcsl_geocentric::vcsl_geocentric(void)
{
  vcsl_axis_sptr a;
  a=new vcsl_axis;
  axes_.push_back(a);
  a=new vcsl_axis(*(vcsl_angle::instance().ptr()));
  axes_.push_back(a);
  a=new vcsl_axis(*(vcsl_angle::instance().ptr()));
  axes_.push_back(a);
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_geocentric::~vcsl_geocentric()
{
}
