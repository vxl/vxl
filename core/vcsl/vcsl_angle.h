#ifndef VCSL_ANGLE_H
#define VCSL_ANGLE_H
//*****************************************************************************
//
// .NAME vcsl_angle - Angle dimension
// .LIBRARY vcsl
// .HEADER  vxl Package
// .INCLUDE vcsl/vcsl_angle.h
// .FILE    vcsl/vcsl_angle.cxx
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/28 François BERTEL Creation. Adapted from IUE
//*****************************************************************************

#include <vcsl/vcsl_angle_ref.h>

#include <vcsl/vcsl_dimension.h>

class vcsl_angle
  : public vcsl_dimension
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************
  
  //: Destructor
  virtual ~vcsl_angle();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `new_unit' a compatible unit for the dimension ?
  virtual bool compatible_unit(const vcsl_unit &new_unit) const;

  //: Return the standard unit associated to the dimension
  virtual vcsl_unit_ref standard_unit(void) const;

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_angle object
  static vcsl_angle_ref instance(void);

protected:
  //: Default constructor
  explicit vcsl_angle(void);

  //: Reference to the unique vcsl_angle object
  static vcsl_angle_ref instance_;
};
#endif // VCSL_ANGLE_H
