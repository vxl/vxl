// This is core/vcsl/vcsl_length_unit.cxx
#include "vcsl_length_unit.h"

#include <vcsl/vcsl_meter.h>

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Are `this' and `other' compatible units ? (Are they associated to the
// same dimension ?)
//---------------------------------------------------------------------------
bool vcsl_length_unit::compatible_units(const vcsl_unit &other) const
{
  return other.cast_to_length_unit()!=0;
}

//---------------------------------------------------------------------------
// Return the standard unit associated to the dimension of `this'
//---------------------------------------------------------------------------
vcsl_unit_sptr vcsl_length_unit::standard_unit(void) const
{
  return vcsl_meter::instance().ptr();
}
