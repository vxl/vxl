// This is core/vcsl/vcsl_axis.cxx
#include "vcsl_axis.h"

#include <vcl_cassert.h>

#include <vcsl/vcsl_dimension.h>
#include <vcsl/vcsl_length.h>
#include <vcsl/vcsl_meter.h>

//---------------------------------------------------------------------------
// Default constructor. Axis with length in meters and an empty label
//---------------------------------------------------------------------------
vcsl_axis::vcsl_axis()
{
  dimension_ = vcsl_length::instance().ptr();
  unit_ = vcsl_meter::instance().ptr();
}

//---------------------------------------------------------------------------
// Constructor from dimension. Unit is the standard one. Label is empty
//---------------------------------------------------------------------------
vcsl_axis::vcsl_axis(vcsl_dimension_sptr const& new_dimension)
{
  dimension_=new_dimension;
  unit_=dimension_->standard_unit();
}

//---------------------------------------------------------------------------
// Constructor from dimension and unit. Label is empty
// REQUIRE: new_dimension.compatible_unit(new_unit)
//---------------------------------------------------------------------------
vcsl_axis::vcsl_axis(vcsl_dimension_sptr const& new_dimension,
                     vcsl_unit_sptr const& new_unit)
{
  // require
  assert(new_dimension->compatible_unit(new_unit));

  dimension_=new_dimension;
  unit_=new_unit;
}

//---------------------------------------------------------------------------
// Set the dimension. The unit is set with the standard unit
//---------------------------------------------------------------------------
void vcsl_axis::set_dimension(vcsl_dimension_sptr const& new_dimension)
{
  dimension_=new_dimension;
  unit_=dimension_->standard_unit();
}

//---------------------------------------------------------------------------
// Set the dimension and the unit
// REQUIRE: new_dimension.compatible_unit(new_unit)
//---------------------------------------------------------------------------
void vcsl_axis::set_dimension_and_unit(vcsl_dimension_sptr const& new_dimension,
                                       vcsl_unit_sptr const& new_unit)
{
  // require
  assert(new_dimension->compatible_unit(new_unit));

  dimension_=new_dimension;
  unit_=new_unit;
}

//---------------------------------------------------------------------------
// Set the unit of the dimension
// REQUIRE dimension()->compatible_unit(new_unit)
//---------------------------------------------------------------------------
void vcsl_axis::set_unit(vcsl_unit_sptr const& new_unit)
{
  // require
  assert(dimension()->compatible_unit(new_unit));

  unit_=new_unit;
}
