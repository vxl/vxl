// This is vxl/vcsl/vcsl_spherical.cxx
#include "vcsl_spherical.h"

#include <vcsl/vcsl_axis.h>
#include <vcsl/vcsl_angle.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

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

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_spherical::~vcsl_spherical()
{
}

//***************************************************************************
// Because VXL does not use dynamic_cast<> :-(
//***************************************************************************

const vcsl_spherical *vcsl_spherical::cast_to_spherical(void) const
{
  return this;
}
