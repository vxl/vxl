// This is core/vcsl/vcsl_angle.cxx
#include "vcsl_angle.h"
#include <vcsl/vcsl_radian.h>

//---------------------------------------------------------------------------
// Is `new_unit' a compatible unit for the dimension ?
//---------------------------------------------------------------------------
bool vcsl_angle::compatible_unit(vcsl_unit_sptr const& new_unit) const
{
  return new_unit->cast_to_angle_unit()!=0;
}

vcsl_unit_sptr vcsl_angle::standard_unit() const
{
  return vcsl_radian::instance().ptr();
}

//---------------------------------------------------------------------------
// Return the reference to the unique vcsl_angle object
//---------------------------------------------------------------------------
vcsl_angle_sptr vcsl_angle::instance()
{
  static vcsl_angle_sptr instance_=new vcsl_angle;
  return instance_;
}
