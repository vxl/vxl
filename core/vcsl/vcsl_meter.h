#ifndef vcsl_meter_h
#define vcsl_meter_h

//:
// \file
// \author François BERTEL
// Modifications
// \verbatim
// 2000/06/28 François BERTEL Creation. Adapted from IUE
// 10/4/2001 Ian Scott (Manchester) Coverted perceps header to doxygen
// \endverbatim


#include <vcsl/vcsl_meter_sptr.h>

#include <vcsl/vcsl_length_unit.h>
//: Standard unit associated to the length dimension
class vcsl_meter
  : public vcsl_length_unit
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Destructor
  virtual ~vcsl_meter();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Returns the number of units of `this' equal of the standard_unit for the dimension
  virtual double units_per_standard_unit(void) const;

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_meter object
  static vcsl_meter_sptr instance(void);
protected:

  //: Default constructor
  explicit vcsl_meter(void);

  //: Reference to the unique vcsl_meter object
  static vcsl_meter_sptr instance_;
};

#endif // vcsl_meter_h
