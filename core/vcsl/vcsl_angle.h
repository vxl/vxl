#ifndef vcsl_angle_h
#define vcsl_angle_h

//:
// \file 
// \brief Angle dimension
// \author François BERTEL
// Modifications
// \verbatim
// 2000/06/28 François BERTEL Creation. Adapted from IUE
// \endverbatim

#include <vcsl/vcsl_angle_sptr.h>

#include <vcsl/vcsl_dimension.h>

//: Angle dimension
class vcsl_angle
  : public vcsl_dimension
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************
  
  //: Destructor
  virtual ~vcsl_angle();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `new_unit' a compatible unit for the dimension ?
  virtual bool compatible_unit(const vcsl_unit &new_unit) const;

  //: Return the standard unit associated to the dimension
  virtual vcsl_unit_sptr standard_unit(void) const;

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_angle object
  static vcsl_angle_sptr instance(void);

protected:
  //: Default constructor
  explicit vcsl_angle(void);

  //: Reference to the unique vcsl_angle object
  static vcsl_angle_sptr instance_;
};

#endif // vcsl_angle_h
