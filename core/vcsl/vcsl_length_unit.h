#ifndef VCSL_LENGTH_UNIT_H
#define VCSL_LENGTH_UNIT_H
//*****************************************************************************
//
// .NAME vcsl_length_unit - Abstract unit associated to the length dimension
// .LIBRARY vcsl
// .HEADER  vcsl/vcsl_length_unit.h
// .INCLUDE vcsl/vcsl_length_unit_ref.h
// .INCLUDE vcsl/vcsl_unit.h
// .FILE    vcsl/vcsl_length_unit.cxx
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/28 François BERTEL Creation. Adapted from IUE
//*****************************************************************************

#include <vcsl/vcsl_length_unit_ref.h>

#include <vcsl/vcsl_unit.h>
 
class vcsl_length_unit
  : public vcsl_unit
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor
  explicit vcsl_length_unit(void);

  //: Destructor
  virtual ~vcsl_length_unit();

  //***************************************************************************
  // Status report
  //***************************************************************************
  
  //: Are `this' and `other' compatible units ? (Are they associated to the
  //: same dimension ?) 
  virtual bool compatible_units(const vcsl_unit &other) const;

  //: Return the standard unit associated to the dimension of `this'
  virtual vcsl_unit_ref standard_unit(void) const;

  //***************************************************************************
  // Because VXL does not use dynamic_cast<> :-(
  //***************************************************************************

  virtual const vcsl_length_unit *cast_to_length_unit(void) const;
};

#endif // #ifndef VCSL_LENGTH_UNIT_H
