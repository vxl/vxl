#ifndef vcsl_millimeter_h_
#define vcsl_millimeter_h_
//:
// \file
#include <vcsl/vcsl_length_unit.h>
#include "vcsl_millimeter_sptr.h"

//: Standard unit associated to the length dimension
class vcsl_millimeter
  : public vcsl_length_unit
{
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor
  vcsl_millimeter() {}

 public:
  // Destructor
  virtual ~vcsl_millimeter() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Returns the number of units of `this' equal of the standard_unit for the dimension
  // Pure virtual function of vcsl_unit
  virtual double units_per_standard_unit() const { return 1000; }

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_millimeter object
  static vcsl_millimeter_sptr instance();
};

#endif // vcsl_millimeter_h_
