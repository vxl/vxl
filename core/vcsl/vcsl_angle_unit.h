#ifndef VCSL_ANGLE_UNIT_H
#define VCSL_ANGLE_UNIT_H
//*****************************************************************************
//
// .NAME vcsl_angle_unit - Abstract unit associated to the angle dimension
// .LIBRARY vcsl
// .HEADER  vxl Package
// .INCLUDE vcsl/vcsl_angle_unit.h
// .FILE    vcsl/vcsl_angle_unit.cxx
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/28 François BERTEL Creation. Adapted from IUE
//*****************************************************************************

#include <vcsl/vcsl_angle_unit_sptr.h>

#include <vcsl/vcsl_unit.h>
 
class vcsl_angle_unit
  : public vcsl_unit
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor
  explicit vcsl_angle_unit(void);

  //: Destructor
  virtual ~vcsl_angle_unit();

  //***************************************************************************
  // Status report
  //***************************************************************************
  
  //: Are `this' and `other' compatible units ? (Are they associated to the
  //: same dimension ?) 
  virtual bool compatible_units(const vcsl_unit &other) const;

  //: Return the standard unit associated to the dimension of `this'
  virtual vcsl_unit_sptr standard_unit(void) const;

  //***************************************************************************
  // Because VXL does not use dynamic_cast<> :-(
  //***************************************************************************

  virtual const vcsl_angle_unit *cast_to_angle_unit(void) const;
};

#endif // #ifndef VCSL_ANGLE_UNIT_H
