// This is vxl/vcsl/vcsl_polar.cxx
#include "vcsl_polar.h"

#include <vcsl/vcsl_axis.h>
#include <vcsl/vcsl_angle.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor.
//---------------------------------------------------------------------------
vcsl_polar::vcsl_polar(void)
{
  vcsl_axis_sptr a;
  a=new vcsl_axis;
  axes_.push_back(a);
  a=new vcsl_axis(*(vcsl_angle::instance().ptr()));
  axes_.push_back(a);
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_polar::~vcsl_polar()
{
}

//***************************************************************************
// Because VXL does not use dynamic_cast<> :-(
//***************************************************************************

const vcsl_polar *vcsl_polar::cast_to_polar(void) const
{
  return this;
}
