// This is core/vcsl/vcsl_angle_unit.cxx
#include "vcsl_angle_unit.h"

#include <vcsl/vcsl_radian.h>

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Are `this' and `other' compatible units ? (Are they associated to the
// same dimension ?)
//---------------------------------------------------------------------------
bool vcsl_angle_unit::compatible_units(const vcsl_unit &other) const
{
  return other.cast_to_angle_unit()!=0;
}

//---------------------------------------------------------------------------
// Return the standard unit associated to the dimension of `this'
//---------------------------------------------------------------------------
vcsl_unit_sptr vcsl_angle_unit::standard_unit(void) const
{
  return vcsl_radian::instance().ptr();
}
