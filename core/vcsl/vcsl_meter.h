#ifndef VCSL_METER_H
#define VCSL_METER_H
//*****************************************************************************
//
// .NAME vcsl_meter - Standard unit associated to the length dimension
// .LIBRARY vcsl
// .HEADER  vcsl/vcsl_meter.h
// .INCLUDE vcsl/vcsl_meter_ref.h
// .INCLUDE vcsl/vcsl_length_unit.h
// .FILE    vcsl/vcsl_meter.cxx
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/28 François BERTEL Creation. Adapted from IUE
//*****************************************************************************

#include <vcsl/vcsl_meter_ref.h>

#include <vcsl/vcsl_length_unit.h>

class vcsl_meter
  : public vcsl_length_unit
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Destructor
  virtual ~vcsl_meter();

  //***************************************************************************
  // Status report
  //***************************************************************************
  
  //: Returns the number of units of `this' equal of the standard_unit for
  //: the dimension
  virtual double units_per_standard_unit(void) const;

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_meter object
  static vcsl_meter_ref instance(void);
protected:

  //: Default constructor
  explicit vcsl_meter(void);
  
  //: Reference to the unique vcsl_meter object
  static vcsl_meter_ref _instance;
};
#endif // #ifndef VCSL_METER_H
