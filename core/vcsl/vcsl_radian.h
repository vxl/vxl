#ifndef VCSL_RADIAN_H
#define VCSL_RADIAN_H
//*****************************************************************************
//
// .NAME vcsl_radian - Standard unit associated to the angle dimension
// .LIBRARY vcsl
// .HEADER  vxl Package
// .INCLUDE vcsl/vcsl_radian.h
// .FILE    vcsl/vcsl_radian.cxx
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/28 François BERTEL Creation. Adapted from IUE
//*****************************************************************************

#include <vcsl/vcsl_radian_ref.h>

#include <vcsl/vcsl_angle_unit.h>

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
  static vcsl_radian_ref instance(void);
protected:

  //: Default constructor
  explicit vcsl_radian(void);
  
  //: Reference to the unique vcsl_radian object
  static vcsl_radian_ref instance_;
};
#endif // #ifndef VCSL_RADIAN_H
