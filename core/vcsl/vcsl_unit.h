#ifndef vcsl_unit_h
#define vcsl_unit_h
//:
// \file
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2004/09/10 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vcsl/vcsl_unit_sptr.h>
#include <vbl/vbl_ref_count.h>

class vcsl_acceleration_unit;
class vcsl_angle_unit;
class vcsl_charge_unit;
class vcsl_length_unit;
class vcsl_mass_unit;
class vcsl_temperature_unit;
class vcsl_time_unit;
class vcsl_velocity_unit;

//: Abstract unit associated to a dimension
// A unit specifies how the quantity is measured--effectively, it specifies
// what is amount is to be defined as 1
//
// This is a pure virutal class.

class vcsl_unit
  : public vbl_ref_count
{
 public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor
  explicit vcsl_unit() {}

  // Copy constructor
  vcsl_unit(vcsl_unit const&) : vbl_ref_count() {}

  // Destructor
  virtual ~vcsl_unit() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Are `this' and `other' compatible units ?
  // (Are they associated to the same dimension ?)
  virtual bool compatible_units(const vcsl_unit &other) const=0;

  //: Return the standard unit associated to the dimension of `this'
  virtual vcsl_unit_sptr standard_unit(void) const=0;

  //: Returns the number of units of `this' equal of the standard_unit for the dimension
  virtual double units_per_standard_unit(void) const=0;

  //***************************************************************************
  // Because VXL does not necessarily use dynamic_cast<>
  //***************************************************************************

  virtual const vcsl_acceleration_unit *cast_to_acceleration_unit(void) const { return 0; }
  virtual const vcsl_angle_unit *cast_to_angle_unit(void) const { return 0; }
  virtual const vcsl_charge_unit *cast_to_charge_unit(void) const { return 0; }
  virtual const vcsl_length_unit *cast_to_length_unit(void) const { return 0; }
  virtual const vcsl_mass_unit *cast_to_mass_unit(void) const { return 0; }
  virtual const vcsl_temperature_unit *cast_to_temperature_unit(void) const { return 0; }
  virtual const vcsl_time_unit *cast_to_time_unit(void) const { return 0; }
  virtual const vcsl_velocity_unit *cast_to_velocity_unit(void) const { return 0; }
};

#endif // vcsl_unit_h
