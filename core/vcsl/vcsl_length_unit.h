#ifndef VCSL_LENGTH_UNIT_H
#define VCSL_LENGTH_UNIT_H

//:
// \file 
// \author François BERTEL
// Modifications
// \verbatim
// 2000/06/28 François BERTEL Creation. Adapted from IUE
// 10/4/2001 Ian Scott (Manchester) Coverted perceps header to doxygen
// \endverbatim


#include <vcsl/vcsl_length_unit_sptr.h>

#include <vcsl/vcsl_unit.h>
//: Abstract unit associated to the length dimension
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
  virtual vcsl_unit_sptr standard_unit(void) const;

  //***************************************************************************
  // Because VXL does not use dynamic_cast<> :-(
  //***************************************************************************

  virtual const vcsl_length_unit *cast_to_length_unit(void) const;
};

#endif // #ifndef VCSL_LENGTH_UNIT_H
