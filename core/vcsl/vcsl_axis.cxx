// This is core/vcsl/vcsl_axis.cxx
#include "vcsl_axis.h"

#include <vcl_cassert.h>

#include <vcsl/vcsl_dimension.h>
#include <vcsl/vcsl_length.h>
#include <vcsl/vcsl_meter.h>

//***************************************************************************
// Constructors/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
// Default constructor. Axis with length in meters and an empty label
//---------------------------------------------------------------------------
vcsl_axis::vcsl_axis(void)
{
  dimension_=vcsl_length::instance().ptr();
  unit_=vcsl_meter::instance().ptr();
}

//---------------------------------------------------------------------------
// Constructor from dimension. Unit is the standard one. Label is empty
//---------------------------------------------------------------------------
vcsl_axis::vcsl_axis(vcsl_dimension &new_dimension)
{
  dimension_=&new_dimension;
  unit_=dimension_->standard_unit();
}

//---------------------------------------------------------------------------
// Constructor from dimension and unit. Label is empty
// REQUIRE: new_dimension.compatible_unit(new_unit)
//---------------------------------------------------------------------------
vcsl_axis::vcsl_axis(vcsl_dimension &new_dimension,
                     vcsl_unit &new_unit)
{
  // require
  assert(new_dimension.compatible_unit(new_unit));

  dimension_=&new_dimension;
  unit_=&new_unit;
}

//---------------------------------------------------------------------------
// Constructor from dimension, unit and label
//---------------------------------------------------------------------------
vcsl_axis::vcsl_axis(vcsl_dimension &new_dimension,
                     vcsl_unit &new_unit,
                     vcl_string &new_label)
{
  dimension_=&new_dimension;
  unit_=&new_unit;
  label_=new_label;
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vcsl_axis::vcsl_axis(const vcsl_axis &other) : vbl_ref_count()
{
  dimension_=other.dimension_;
  unit_=other.unit_;
  label_=other.label_;
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vcsl_axis::~vcsl_axis()
{
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// Return the dimension
//---------------------------------------------------------------------------
const vcsl_dimension_sptr vcsl_axis::dimension(void) const
{
  return dimension_;
}

//---------------------------------------------------------------------------
// Return the unit of the dimension
//---------------------------------------------------------------------------
const vcsl_unit_sptr vcsl_axis::unit(void) const
{
  return unit_;
}

//---------------------------------------------------------------------------
// Return the label of the axis
//---------------------------------------------------------------------------
const vcl_string vcsl_axis::label(void) const
{
  return label_;
}

//***************************************************************************
// Status change
//***************************************************************************

//---------------------------------------------------------------------------
// Set the dimension. The unit is set with the standard unit
//---------------------------------------------------------------------------
void vcsl_axis::set_dimension(vcsl_dimension &new_dimension)
{
  dimension_=&new_dimension;
  unit_=dimension_->standard_unit();
}

//---------------------------------------------------------------------------
// Set the dimension and the unit
// REQUIRE: new_dimension.compatible_unit(new_unit)
//---------------------------------------------------------------------------
void vcsl_axis::set_dimension_and_unit(vcsl_dimension &new_dimension,
                                       vcsl_unit &new_unit)
{
  // require
  assert(new_dimension.compatible_unit(new_unit));

  dimension_=&new_dimension;
  unit_=&new_unit;
}

//---------------------------------------------------------------------------
// Set the unit of the dimension
// REQUIRE dimension()->compatible_unit(new_unit)
//---------------------------------------------------------------------------
void vcsl_axis::set_unit(vcsl_unit &new_unit)
{
  // require
  assert(dimension()->compatible_unit(new_unit));

  unit_=&new_unit;
}

//---------------------------------------------------------------------------
// Set the label
//---------------------------------------------------------------------------
void vcsl_axis::set_label(const vcl_string &new_label)
{
  label_=new_label;
}
