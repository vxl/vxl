#ifndef VCSL_DEGREE_H
#define VCSL_DEGREE_H
//*****************************************************************************
//
// .NAME vcsl_degree - A unit associated to the angle dimension
// .LIBRARY vcsl
// .HEADER  vcsl/vcsl_degree.h
// .INCLUDE vcsl/vcsl_degree_ref.h
// .INCLUDE vcsl/vcsl_angle_unit.h
// .FILE    vcsl/vcsl_degree.cxx
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/28 François BERTEL Creation. Adapted from IUE
//*****************************************************************************

#include <vcsl/vcsl_degree_ref.h>

#include <vcsl/vcsl_angle_unit.h>

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
  static vcsl_degree_ref instance(void);
protected:

  //: Default constructor
  explicit vcsl_degree(void);
  
  //: Reference to the unique vcsl_degree object
  static vcsl_degree_ref _instance;
};
#endif // #ifndef VCSL_DEGREE_H
