#ifndef vcsl_angle_unit_h
#define vcsl_angle_unit_h

//:
// \file
// \brief Abstract unit associated to the angle dimension
// \author François BERTEL
// Modifications
// \verbatim
// 2000/06/28 François BERTEL Creation. Adapted from IUE
// \endverbatim

#include <vcsl/vcsl_angle_unit_sptr.h>
#include <vcsl/vcsl_unit.h>

//: Abstract unit associated to the angle dimension
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

  //: Are `this' and `other' compatible units ?
  // (Are they associated to the same dimension ?)
  virtual bool compatible_units(const vcsl_unit &other) const;

  //: Return the standard unit associated to the dimension of `this'
  virtual vcsl_unit_sptr standard_unit(void) const;

  //***************************************************************************
  // Because VXL does not use dynamic_cast<> :-(
  //***************************************************************************

  virtual const vcsl_angle_unit *cast_to_angle_unit(void) const;
};

#endif // vcsl_angle_unit_h
