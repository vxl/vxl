// This is vxl/vcsl/vcsl_unit.cxx
#include "vcsl_unit.h"

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor
//---------------------------------------------------------------------------
vcsl_unit::vcsl_unit(void)
{
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_unit::~vcsl_unit()
{
}

//***************************************************************************
// Because VXL does not use dynamic_cast<> :-(
//***************************************************************************
const vcsl_acceleration_unit *
vcsl_unit::cast_to_acceleration_unit(void) const
{
  return 0;
}

const vcsl_angle_unit *vcsl_unit::cast_to_angle_unit(void) const
{
  return 0;
}

const vcsl_charge_unit *vcsl_unit::cast_to_charge_unit(void) const
{
  return 0;
}

const vcsl_length_unit *vcsl_unit::cast_to_length_unit(void) const
{
  return 0;
}

const vcsl_mass_unit *vcsl_unit::cast_to_mass_unit(void) const
{
  return 0;
}

const vcsl_temperature_unit *vcsl_unit::cast_to_temperature_unit(void) const
{
  return 0;
}

const vcsl_time_unit *vcsl_unit::cast_to_time_unit(void) const
{
  return 0;
}

const vcsl_velocity_unit *vcsl_unit::cast_to_velocity_unit(void) const
{
  return 0;
}
