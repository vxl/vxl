#ifndef VCSL_UNIT_H
#define VCSL_UNIT_H
//*****************************************************************************
//
// .NAME vcsl_unit - Abstract unit associated to a dimension
// .LIBRARY vcsl
// .HEADER  vxl Package
// .INCLUDE vcsl/vcsl_unit.h
// .FILE    vcsl/vcsl_unit.cxx
//
// .SECTION Description
// A unit specifies how the quantity is measured--effectively, it specifies
// what is amount is to be defined as 1
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/28 François BERTEL Creation. Adapted from IUE
//*****************************************************************************

#include <vcsl/vcsl_unit_ref.h>

#include <vbl/vbl_ref_count.h>

// Because VXL does not use dynamic_cast<> :-(
class vcsl_acceleration_unit;
class vcsl_angle_unit;
class vcsl_charge_unit;
class vcsl_length_unit;
class vcsl_mass_unit;
class vcsl_temperature_unit;
class vcsl_time_unit;
class vcsl_velocity_unit;

class vcsl_unit
  : public vbl_ref_count
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************
  
  //: Default constructor
  explicit vcsl_unit(void);

  //: Destructor
  virtual ~vcsl_unit();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Are `this' and `other' compatible units ? (Are they associated to the
  //: same dimension ?) 
  virtual bool compatible_units(const vcsl_unit &other) const=0;

  //: Return the standard unit associated to the dimension of `this'
  virtual vcsl_unit_ref standard_unit(void) const=0;

  //: Returns the number of units of `this' equal of the standard_unit for
  //: the dimension
  virtual double units_per_standard_unit(void) const=0;

  //***************************************************************************
  // Because VXL does not use dynamic_cast<> :-(
  //***************************************************************************

  virtual const vcsl_acceleration_unit *cast_to_acceleration_unit(void) const;
  virtual const vcsl_angle_unit *cast_to_angle_unit(void) const;
  virtual const vcsl_charge_unit *cast_to_charge_unit(void) const;  
  virtual const vcsl_length_unit *cast_to_length_unit(void) const;
  virtual const vcsl_mass_unit *cast_to_mass_unit(void) const;
  virtual const vcsl_temperature_unit *cast_to_temperature_unit(void) const;
  virtual const vcsl_time_unit *cast_to_time_unit(void) const;
  virtual const vcsl_velocity_unit *cast_to_velocity_unit(void) const;
};

#endif // #ifndef VCSL_UNIT_H
