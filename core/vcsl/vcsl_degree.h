#ifndef vcsl_degree_h_
#define vcsl_degree_h_
//:
// \file
// \brief A unit associated to the angle dimension
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/06/28 François BERTEL Creation. Adapted from IUE
//   2004/09/17 Peter Vanroose  made "instance_" a local static variable of method instance()
// \endverbatim

#include <vcsl/vcsl_angle_unit.h>
#include <vcsl/vcsl_degree_sptr.h>

//: A unit associated to the angle dimension
//  See also \b vcsl_radian
class vcsl_degree
  : public vcsl_angle_unit
{
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor
  vcsl_degree() {}

 public:
  // Destructor
  virtual ~vcsl_degree() {}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Returns the number of units of `this' equal of the standard_unit for the dimension
  // Pure virtual function of vcsl_unit
  virtual double units_per_standard_unit() const;

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_degree object
  static vcsl_degree_sptr instance();
};

#endif // vcsl_degree_h_
