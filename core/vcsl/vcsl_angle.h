#ifndef vcsl_angle_h_
#define vcsl_angle_h_
//:
// \file
// \brief Angle dimension
// \author Francois BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 Francois BERTEL Creation. Adapted from IUE
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
  vcsl_angle() = default;

 public:
  // Destructor
  ~vcsl_angle() override = default;

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `new_unit' a compatible unit for the dimension ?
  // Pure virtual function of vcsl_dimension
  bool compatible_unit(vcsl_unit_sptr const& new_unit) const override;

  //: Return the standard unit associated to the dimension
  // Pure virtual function of vcsl_dimension
  vcsl_unit_sptr standard_unit() const override;

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_angle object
  static vcsl_angle_sptr instance();
};

#endif // vcsl_angle_h_
