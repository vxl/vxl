#ifndef vcsl_angle_unit_h_
#define vcsl_angle_unit_h_
//:
// \file
// \brief Abstract unit associated to the angle dimension
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2004/09/10 Peter Vanroose  Inlined all 1-line methods in class decl
//   2004/09/17 Peter Vanroose  do not pass vcsl_unit objects; use vcsl_unit_sptr instead
// \endverbatim

#include <vcsl/vcsl_unit.h>
#include <vcsl/vcsl_angle_unit_sptr.h>
#include <vcsl/vcsl_unit_sptr.h>

//: Abstract unit associated to the angle dimension
class vcsl_angle_unit
  : public vcsl_unit
{
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

 protected:
  // Default constructor
  vcsl_angle_unit() {}

 public:
  // Destructor
  virtual ~vcsl_angle_unit() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Are `this' and `other' compatible units ?
  // (Are they associated to the same dimension ?)
  // Pure virtual function of vcsl_unit
  virtual bool compatible_units(vcsl_unit_sptr const& other) const;

  //: Return the standard unit associated to the dimension of `this'
  // Pure virtual function of vcsl_unit
  virtual vcsl_unit_sptr standard_unit() const;

  //***************************************************************************
  // Because VXL does not necessarily use dynamic_cast<>
  //***************************************************************************

  virtual const vcsl_angle_unit *cast_to_angle_unit() const { return this; }
};

#endif // vcsl_angle_unit_h_
