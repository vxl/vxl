#ifndef vcsl_meter_h_
#define vcsl_meter_h_
//:
// \file
// \brief Standard unit associated to the length dimension
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2004/09/17 Peter Vanroose  made "instance_" a local static variable of method instance()
// \endverbatim

#include <vcsl/vcsl_length_unit.h>
#include <vcsl/vcsl_meter_sptr.h>

//: Standard unit associated to the length dimension
class vcsl_meter
  : public vcsl_length_unit
{
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor
  vcsl_meter() {}

 public:
  // Destructor
  virtual ~vcsl_meter() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Returns the number of units of `this' equal of the standard_unit for the dimension
  // Pure virtual function of vcsl_unit
  virtual double units_per_standard_unit() const { return 1; }

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_meter object
  static vcsl_meter_sptr instance();
};

#endif // vcsl_meter_h_
