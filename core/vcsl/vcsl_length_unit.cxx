// This is vxl/vcsl/vcsl_length_unit.cxx
#include "vcsl_length_unit.h"

#include <vcsl/vcsl_meter.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_length_unit::vcsl_length_unit(void)
{
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_length_unit::~vcsl_length_unit()
{
}

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

//***************************************************************************
// Because VXL does not use dynamic_cast<> :-(
//***************************************************************************
const vcsl_length_unit *vcsl_length_unit::cast_to_length_unit(void) const
{
  return this;
}
