#ifndef vcsl_length_unit_h
#define vcsl_length_unit_h
//:
// \file
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2004/09/10 Peter Vanroose  Inlined all 1-line methods in class decl
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
  explicit vcsl_length_unit() {}

  //: Destructor
  virtual ~vcsl_length_unit() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Are `this' and `other' compatible units ?
  // (Are they associated to the same dimension ?)
  virtual bool compatible_units(const vcsl_unit &other) const;

  //: Return the standard unit associated to the dimension of `this'
  virtual vcsl_unit_sptr standard_unit(void) const;

  //***************************************************************************
  // Because VXL does not necessarily use dynamic_cast<>
  //***************************************************************************

  virtual const vcsl_length_unit *cast_to_length_unit(void) const {return this;}
};

#endif // vcsl_length_unit_h
