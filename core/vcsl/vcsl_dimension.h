#ifndef VCSL_DIMENSION_H
#define VCSL_DIMENSION_H
//*****************************************************************************
//
// .NAME vcsl_dimension - Abstract dimension
// .LIBRARY vcsl
// .HEADER  vcsl/vcsl_dimension.h
// .INCLUDE vcsl/vcsl_dimension_ref.h
// .INCLUDE vbl/vbl_ref_count.h
// .INCLUDE vcsl/vcsl_unit_ref.h
// .FILE    vcsl/vcsl_dimension.cxx
//
// .SECTION Description
// A dimension is a type of quantity to be measured
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/28 François BERTEL Creation. Adapted from IUE
//*****************************************************************************

#include <vcsl/vcsl_dimension_ref.h>

#include <vbl/vbl_ref_count.h>

#include <vcsl/vcsl_unit_ref.h>

class vcsl_dimension
  : public vbl_ref_count
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************
  
  //: Default constructor
  explicit vcsl_dimension(void);

  //: Destructor
  virtual ~vcsl_dimension();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `new_unit' a compatible unit for the dimension ?
  virtual bool compatible_unit(const vcsl_unit &new_unit) const=0;

  //: Return the standard unit associated to the dimension
  virtual vcsl_unit_ref standard_unit(void) const=0;
};

#endif // #ifndef VCSL_DIMENSION_H
