#include <vcsl/vcsl_axis.h>

#include <vcl/vcl_cassert.h>

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
  _dimension=vcsl_length::instance().ptr();
  _unit=vcsl_meter::instance().ptr();
}

//---------------------------------------------------------------------------
// Constructor from dimension. Unit is the standard one. Label is empty
//---------------------------------------------------------------------------
vcsl_axis::vcsl_axis(vcsl_dimension &new_dimension)
{
  _dimension=&new_dimension;
  _unit=_dimension->standard_unit();
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

  _dimension=&new_dimension;
  _unit=&new_unit;
}

//---------------------------------------------------------------------------
// Constructor from dimension, unit and label
//---------------------------------------------------------------------------
vcsl_axis::vcsl_axis(vcsl_dimension &new_dimension,
                     vcsl_unit &new_unit,
                     vcl_string &new_label)
{
  _dimension=&new_dimension;
  _unit=&new_unit;
  _label=new_label;
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vcsl_axis::vcsl_axis(const vcsl_axis &other)
{
  _dimension=other._dimension;
  _unit=other._unit;
  _label=other._label;
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
const vcsl_dimension_ref vcsl_axis::dimension(void) const
{
  return _dimension;
}

//---------------------------------------------------------------------------
// Return the unit of the dimension
//---------------------------------------------------------------------------
const vcsl_unit_ref vcsl_axis::unit(void) const
{
  return _unit;
}

//---------------------------------------------------------------------------
// Return the label of the axis
//---------------------------------------------------------------------------
const vcl_string vcsl_axis::label(void) const
{
  return _label;
}

//***************************************************************************
// Status change
//***************************************************************************

//---------------------------------------------------------------------------
// Set the dimension. The unit is set with the standard unit
//---------------------------------------------------------------------------
void vcsl_axis::set_dimension(vcsl_dimension &new_dimension)
{
  _dimension=&new_dimension;
  _unit=_dimension->standard_unit();
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

  _dimension=&new_dimension;
  _unit=&new_unit;
}

//---------------------------------------------------------------------------
// Set the unit of the dimension
// REQUIRE dimension()->compatible_unit(new_unit)
//---------------------------------------------------------------------------
void vcsl_axis::set_unit(vcsl_unit &new_unit)
{
  // require
  assert(dimension()->compatible_unit(new_unit));

  _unit=&new_unit;
}

//---------------------------------------------------------------------------
// Set the label
//---------------------------------------------------------------------------
void vcsl_axis::set_label(const vcl_string &new_label)
{
  _label=new_label;
}
