#ifndef VCSL_DIMENSION_H
#define VCSL_DIMENSION_H

//:
// \file 
// \author François BERTEL
// Modifications
// \verbatim
// 2000/06/28 François BERTEL Creation. Adapted from IUE
// \endverbatim

#include <vcsl/vcsl_dimension_sptr.h>

#include <vbl/vbl_ref_count.h>

#include <vcsl/vcsl_unit_sptr.h>
//: Abstract dimension
// A dimension is a type of quantity to be measured
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
  virtual vcsl_unit_sptr standard_unit(void) const=0;
};

#endif // #ifndef VCSL_DIMENSION_H
