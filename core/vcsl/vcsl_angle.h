#ifndef vcsl_angle_h_
#define vcsl_angle_h_
//:
// \file
// \brief Angle dimension
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2004/09/17 Peter Vanroose  do not pass vcsl_unit objects; use vcsl_unit_sptr instead
// \endverbatim

#include <vcsl/vcsl_dimension.h>
#include <vcsl/vcsl_angle_sptr.h>
#include <vcsl/vcsl_unit_sptr.h>

//: Angle dimension
class vcsl_angle
  : public vcsl_dimension
{
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor
  vcsl_angle() {}

 public:
  // Destructor
  virtual ~vcsl_angle() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `new_unit' a compatible unit for the dimension ?
  // Pure virtual function of vcsl_dimension
  virtual bool compatible_unit(vcsl_unit_sptr const& new_unit) const;

  //: Return the standard unit associated to the dimension
  // Pure virtual function of vcsl_dimension
  virtual vcsl_unit_sptr standard_unit() const;

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_angle object
  static vcsl_angle_sptr instance();
};

#endif // vcsl_angle_h_
