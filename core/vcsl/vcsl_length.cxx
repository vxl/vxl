// This is core/vcsl/vcsl_length.cxx
#include "vcsl_length.h"
#include <vcsl/vcsl_meter.h>

//---------------------------------------------------------------------------
// Is `new_unit' a compatible unit for the dimension ?
//---------------------------------------------------------------------------
bool vcsl_length::compatible_unit(vcsl_unit_sptr const& new_unit) const
{
  return new_unit->cast_to_length_unit()!=0;
}

//---------------------------------------------------------------------------
// Return the standard unit associated to the dimension
//---------------------------------------------------------------------------
vcsl_unit_sptr vcsl_length::standard_unit() const
{
  return vcsl_meter::instance().ptr();
}

//---------------------------------------------------------------------------
// Return the reference to the unique vcsl_length object
//---------------------------------------------------------------------------
vcsl_length_sptr vcsl_length::instance()
{
  static vcsl_length_sptr instance_=new vcsl_length;
  return instance_;
}
