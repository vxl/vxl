#ifndef vcsl_radian_h_
#define vcsl_radian_h_
//:
// \file
// \brief Standard unit associated to the angle dimension
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2004/09/17 Peter Vanroose  made "instance_" a local static variable of method instance()
// \endverbatim

#include <vcsl/vcsl_angle_unit.h>
#include <vcsl/vcsl_radian_sptr.h>

//: Standard unit associated to the angle dimension
//  See also \b vcsl_degree
class vcsl_radian
  : public vcsl_angle_unit
{
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor
  vcsl_radian() {}

 public:
  // Destructor
  virtual ~vcsl_radian() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Returns the number of units of `this' equal of the standard_unit for the dimension
  // Pure virtual function of vcsl_unit
  virtual double units_per_standard_unit() const { return 1; }

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_radian object
  static vcsl_radian_sptr instance();
};

#endif // vcsl_radian_h_
