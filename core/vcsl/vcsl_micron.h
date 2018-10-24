#ifndef vcsl_micron_h_
#define vcsl_micron_h_
//:
// \file
#include <vcsl/vcsl_length_unit.h>
#include "vcsl_micron_sptr.h"

//: Standard unit associated to the length dimension
class vcsl_micron
  : public vcsl_length_unit
{
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor
  vcsl_micron() = default;

 public:
  // Destructor
  ~vcsl_micron() override = default;

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Returns the number of units of `this' equal of the standard_unit for the dimension
  // Pure virtual function of vcsl_unit
  double units_per_standard_unit() const override { return 1000000; }

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_micron object
  static vcsl_micron_sptr instance();
};

#endif // vcsl_micron_h_
