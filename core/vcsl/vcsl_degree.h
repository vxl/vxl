#ifndef VCSL_DEGREE_H
#define VCSL_DEGREE_H

//:
// \file 
// \author François BERTEL
// Modifications
// \verbatim
// 2000/06/28 François BERTEL Creation. Adapted from IUE
// \endverbatim


#include <vcsl/vcsl_degree_sptr.h>

#include <vcsl/vcsl_angle_unit.h>
//: A unit associated to the angle dimension
class vcsl_degree
  : public vcsl_angle_unit
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Destructor
  virtual ~vcsl_degree();

  //***************************************************************************
  // Status report
  //***************************************************************************
  
  //: Returns the number of units of `this' equal of the standard_unit for
  //: the dimension
  virtual double units_per_standard_unit(void) const;

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_degree object
  static vcsl_degree_sptr instance(void);
protected:

  //: Default constructor
  explicit vcsl_degree(void);
  
  //: Reference to the unique vcsl_degree object
  static vcsl_degree_sptr instance_;
};
#endif // #ifndef VCSL_DEGREE_H
