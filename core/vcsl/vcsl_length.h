#ifndef VCSL_LENGTH_H
#define VCSL_LENGTH_H
//*****************************************************************************
//
// .NAME vcsl_length - Length dimension
// .LIBRARY vcsl
// .HEADER  vcsl/vcsl_length.h
// .INCLUDE vcsl/vcsl_length_ref.h
// .INCLUDE vcsl/vcsl_dimension.h
// .FILE    vcsl/vcsl_length.cxx
//
// .SECTION Author
// François BERTEL
//
// .SECTION Modifications
// 2000/06/28 François BERTEL Creation. Adapted from IUE
//*****************************************************************************

#include <vcsl/vcsl_length_ref.h>

#include <vcsl/vcsl_dimension.h>

class vcsl_length
  : public vcsl_dimension
{
public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************
  
  //: Destructor
  virtual ~vcsl_length();

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Is `new_unit' a compatible unit for the dimension ?
  virtual bool compatible_unit(const vcsl_unit &new_unit) const;

  //: Return the standard unit associated to the dimension
  virtual vcsl_unit_ref standard_unit(void) const;

  //***************************************************************************
  // Singleton pattern
  //***************************************************************************

  //: Return the reference to the unique vcsl_length object
  static vcsl_length_ref instance(void);

protected:
  //: Default constructor
  explicit vcsl_length(void);

  //: Reference to the unique vcsl_length object
  static vcsl_length_ref _instance;
};
#endif // VCSL_LENGTH_H
