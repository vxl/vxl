#ifndef VCSL_RADIAN_H
#define VCSL_RADIAN_H

//:
// \file 
// \author François BERTEL
// Modifications
// \verbatim
// 2000/06/28 François BERTEL Creation. Adapted from IUE
// 10/4/2001 Ian Scott (Manchester) Coverted perceps header to doxygen
// \endverbatim


#include <vcsl/vcsl_radian_sptr.h>

#include <vcsl/vcsl_angle_unit.h>
//: Standard unit associated to the angle dimension
class vcsl_radian
  : public vcsl_angle_unit
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Destructor
  virtual ~vcsl_radian();

  //***************************************************************************
  // Status report
  //***************************************************************************
  
  //: Returns the number of units of `this' equal of the standard_unit for
  //: the dimension
  virtual double units_per_standard_unit(void) const;

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_radian object
  static vcsl_radian_sptr instance(void);
protected:

  //: Default constructor
  explicit vcsl_radian(void);
  
  //: Reference to the unique vcsl_radian object
  static vcsl_radian_sptr instance_;
};
#endif // #ifndef VCSL_RADIAN_H
